from pydantic import BaseModel
from typing import Optional, List
class ExtraExaminer(BaseModel):
    """
    Informaçẽos dos professores extra (que não são orientadores).

    Os atributos foram nomeados em português visando a melhor concordância com o tipo de payload que o algoritmo espera.
    """
    nome: str
    grandeArea: Optional[str]
    area: Optional[str]
    subArea: Optional[str]