#!/bin/bash

python -m venv .venv
source .venv/bin/activate
python -m pip install wheel
python -m pip install adafruit-nrfutil
python -m pip install Pillow
python -m pip install -r tools/mcuboot/requirements.txt

git submodule update --init

npm install lv_font_conv
