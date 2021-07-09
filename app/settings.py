import os
from os.path import join, dirname
from dotenv import load_dotenv

dotenv_path = join(dirname(__file__), '.env')
load_dotenv(dotenv_path)

EMAIL_PASS = os.environ.get("EMAIL_PASS")
EMAIL_SENDER = os.environ.get("EMAIL_SENDER")