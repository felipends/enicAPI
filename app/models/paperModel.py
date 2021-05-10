from pydantic import BaseModel
from typing import Optional, List

class Paper(BaseModel):
    """
    Informações dos trabalhos a serem apresentados.

    Os atributos foram nomeados em português visando a melhor concordância com o tipo de payload que o algoritmo espera.
    """
    aluno: str
    orientador: str
    grandeArea: Optional[str]
    area: Optional[str]
    subArea: Optional[str]
    linkVideo: Optional[str]
    resumo: Optional[str]
    titulo: Optional[str]