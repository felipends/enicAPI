from pydantic import BaseModel
from typing import Optional, List
from .paperModel import Paper
from .extraExaminerModel import ExtraExaminer

class EnicDay(BaseModel):
    """
    Informações de um dia do Enic para compor uma
    instância de dados de entrada para a heurística.
    Local da instancia,
    Número Turnos,
    Número de salas,
    Trabalhos por sala,
    Duração da apresentação,
    Horário de início de cada turno
    Professores extra daquele campus

    Os atributos foram nomeados em português visando a melhor concordância com o tipo de payload que o algoritmo espera.
    """
    localEnic: str
    dataEnic: str
    numTurnos: int
    horaInicio: List[str]
    numSalas: int
    numTrabalhosPorSala: int
    duracaoTrabalho: int # Tempo de apresentação de cada trabalho
    userEmail: str
    trabalhos: List[Paper]
    professoresExtra: Optional[List[ExtraExaminer]]