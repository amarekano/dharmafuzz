#!/usr/bin/env python
# encoding: utf-8

import os
import random
import struct
import sys
import dharma

from dharma.core.dharma import DharmaMachine

grammar_files = []
prefix_data = '' 
suffix_data = '' 
template_data = '' 
dharma = ''
grammars = [
	'JSArray.dg',
	'JSString.dg',
	'JSObject.dg'
]

def init():
	global dharma
	sys.setrecursionlimit(20000)
	seed = struct.unpack('q', os.urandom(8))[0]
	random.seed(seed)
	dharma = DharmaMachine(prefix_data, suffix_data, template_data)
	settings = open(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'settings.py'))
	dharma.process_settings(settings)
	for grammar in grammars:
		grammar_files.append(open(os.path.join(os.path.dirname(os.path.abspath(__file__)), grammar)))
	dharma.process_grammars(grammar_files)

def custom_mutator(data, max_size, seed):
	payload = bytearray(dharma.generate_content().encode())
	if len(payload) > max_size:
		return payload[0:max_size]
	return payload
