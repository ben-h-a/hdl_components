from __future__ import annotations
import argparse
import json
import logging
from config_reg import RegConfig, InputPort, OutputPort, RegDef

arg_parser = argparse.ArgumentParser()

arg_parser.add_argument("-j", "--json", required=True, default=None, help="JSON configuration file for register definition")

arg_parser.add_argument("-ll", '--log_level', required=False, default=logging.INFO, help="Log level override, valid options: DEBUG, INFO, WARNING, ERROR")
arg_parser.add_argument("-o", "--output_dir", required=False, default=None, help="output directory override")


args = arg_parser.parse_args()

with open(args.json, "r", encoding="utf-8") as j_file:
    json_data = json.load(j_file)

config = RegConfig(json_data)

