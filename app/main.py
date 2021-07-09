import os
import subprocess
import threading
from fastapi import FastAPI
from app.models.enicDayModel import EnicDay

import smtplib, ssl, threading
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
from email.mime.base import MIMEBase
from email import encoders

from app.settings import *

app = FastAPI()

def generateDictionary(dayInfo):
    """
    Gera representação legível dos dados das instâncias do problema.
    """
    authorsList = []
    topicsList = []

    for examiner in dayInfo.professoresExtra:
        authorsList.append(examiner.nome)

    for paper in dayInfo.trabalhos:
        authorsList.append(paper.aluno)
        authorsList.append(paper.orientador)
        topicsList.append(paper.grandeArea)
        topicsList.append(paper.area)
        topicsList.append(paper.subArea)

    authorsList = list(set(authorsList))
    topicsList = list(set(topicsList))

    return authorsList, topicsList

def dictionaryToFile(dictFilename, authors, topics):
    """
    Escreve os dados do dicionário em um arquivo a ser consumido pelo resolvedor.
    """
    dictFile = open(dictFilename, "w+")

    idx = 0
    for author in authors:
        dictFile.write("{} {}\n".format(idx, author))
        idx += 1
    dictFile.write("\n")

    idx = 0
    for topic in topics:
        dictFile.write("{} {}\n".format(idx, topic))
        idx += 1

    dictFile.close()

def generateInstance(dayInfo, dayDict):
    """
    Estrutura da instancia
    Número de topicos
    ex: 10 0
    Número de autores
    ex: 80 0
    Número de trabalhos
    ex: 50 0
    Info de cada trabalho
    (número de topicos, indices dos topicos, numero de autores, indices dos autores)
    ex: 3 1 2 4 2 0 1
    Número de sessões
    ex: 3
    Info de cada sessão
    (número de trabalhos na sessão, duração em minutos de cadaapresentação, data (dd mm aaaa), hora e minuto iniciais)
    ex: 12 20 15 10 2018 14 0
    0
    0
    Núemro de professores extra
    ex: 3
    Info de cada professor extra
    (indice, numero de topicos e indices dos topicos)
    ex: 1 2 1 4
    """

    instanceStr = "{} 0\n".format(len(dayDict[1]))
    instanceStr += "{} 0\n".format(len(dayDict[0]))
    instanceStr += "{} 0\n".format(len(dayInfo.trabalhos))

    for trab in dayInfo.trabalhos:
        trabTopicos = []
        if trab.grandeArea:
            trabTopicos.append(trab.grandeArea)
        if trab.area:
            trabTopicos.append(trab.area)
        if trab.subArea:
            trabTopicos.append(trab.subArea)
        instanceStr += "{}".format(len(trabTopicos))
        for top in trabTopicos:
            instanceStr += " {}".format(dayDict[1].index(top))

        instanceStr += " 2 {} {}".format(dayDict[0].index(trab.aluno), dayDict[0].index(trab.orientador))
        instanceStr += "\n"
    numSessoes = dayInfo.numTurnos * dayInfo.numSalas
    instanceStr += "{}\n".format(numSessoes)

    for hora in dayInfo.horaInicio:
        for i in range(dayInfo.numSalas):
            instanceStr += "{} {} {} {} {} {} {}\n".format(dayInfo.numTrabalhosPorSala, dayInfo.duracaoTrabalho, dayInfo.dataEnic.split("/")[0], dayInfo.dataEnic.split("/")[1], dayInfo.dataEnic.split("/")[2], hora.split(":")[0], hora.split(":")[1])

    instanceStr += "0\n0\n"

    instanceStr += "{}\n".format(len(dayInfo.professoresExtra))
    for prof in dayInfo.professoresExtra:
        instanceStr += "{}".format(dayDict[0].index(prof.nome))

        profTopics = []
        if prof.grandeArea and prof.grandeArea in dayDict[1]:
            profTopics.append(prof.grandeArea)
        if prof.area and prof.area in dayDict[1]:
            profTopics.append(prof.area)
        if prof.subArea and prof.subArea in dayDict[1]:
            profTopics.append(prof.subArea)

        instanceStr += " {}".format(len(profTopics))
        for top in profTopics:
            if top in dayDict[1]:
                instanceStr += " {}".format(dayDict[1].index(top))


        instanceStr += "\n"

    return instanceStr

def runTaskAndNotify(userEmail):
    subprocess.call(["./services/heuristica/heuristic","./instance.txt","./result.json","./dict.txt"])

    port = 465
    password = EMAIL_PASS

    context = ssl.create_default_context()

    msg = MIMEMultipart()
    msg['From'] = EMAIL_SENDER
    msg['To'] = userEmail
    msg['Subject'] = 'ENIC Scheduler: sua solução está pronta'
    mail_body = 'Verifique a solução para a programação do ENIC que segue no arquivo anexo.'

    msg.attach(MIMEText(mail_body, 'plain'))

    filename = 'result.json'
    attachment = open('./'+filename, 'rb')

    p = MIMEBase('application', 'octet-stream')

    p.set_payload(attachment.read())

    encoders.encode_base64(p)

    p.add_header('Content-Disposition', 'attachment; filename=%s' % filename)

    msg.attach(p)

    with smtplib.SMTP_SSL("smtp.gmail.com", port, context = context) as server:
        server.login("dasilvajfelipe@gmail.com", password)
        server.sendmail("dasilvajfelipe@gmail.com", "felipendsdev@gmail.com", msg.as_string())
        server.quit()

    os.remove("./instance.txt")
    os.remove("./result.json")
    os.remove("./dict.txt")


@app.get("/")
def read_root():
    """Test path"""
    return {"Test": "OK"}

@app.post("/schedule/")
def store_day(dayInfo: EnicDay):
    """
    Caminho para realizar programação de um dia do evento.
    """
    dictFilename = "./dict.txt"
    instanceFilename = "./instance.txt"
    resultFilename = "./result.json"

    authors, topics = generateDictionary(dayInfo)
    dictionaryToFile(dictFilename, authors, topics)

    instanceStr = generateInstance(dayInfo, (authors, topics))
    instanceFile = open(instanceFilename, "w+")
    resultFile = open(resultFilename, "w+")
    resultFile.close()
    instanceFile.write(instanceStr)
    instanceFile.close()

    threading.Thread(target=runTaskAndNotify, daemon=True, args=[dayInfo.userEmail]).start()
   
    data = {"message": "job created!"}

    return data
