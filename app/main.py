import os
import json
import subprocess
import time
from fastapi import FastAPI, HTTPException
from pydantic import BaseModel
from app.models.paperModel import Paper
from app.models.enicDayModel import EnicDay
from app.models.extraExaminerModel import ExtraExaminer

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

    # TODO: make this process run asynchronous and then notify with the result
    process_returncode = subprocess.call(["./services/heuristica/heuristic","./instance.txt","./result.json","./dict.txt"])
    if process_returncode:
        raise HTTPException(status_code=400, detail="Bad Request: Error trying to process the given instance.")

    data = {}
    with open(resultFilename, "r") as outfile:
        data = json.load(outfile)
    data['local'] = dayInfo.localEnic
    papers = dayInfo.trabalhos

    for session in data['sessions']:
        for paper in session['papers']:
            paper['grande_area'] = next((x for x in papers if x.aluno == paper['aluno']), None).grandeArea
            paper['area'] = next((x for x in papers if x.aluno == paper['aluno']), None).area
            paper['sub_area'] = next((x for x in papers if x.aluno == paper['aluno']), None).subArea
            if next((x for x in papers if x.aluno == paper['aluno']), None).linkVideo:
                paper['link_video'] = next((x for x in papers if x.aluno == paper['aluno']), None).linkVideo
            if next((x for x in papers if x.aluno == paper['aluno']), None).resumo:
                paper['resumo'] = next((x for x in papers if x.aluno == paper['aluno']), None).resumo
            if next((x for x in papers if x.aluno == paper['aluno']), None).titulo:
                paper['titulo'] = next((x for x in papers if x.aluno == paper['aluno']), None).titulo

    os.remove(resultFilename)
    os.remove(dictFilename)
    # os.remove(instanceFilename)

    return data
