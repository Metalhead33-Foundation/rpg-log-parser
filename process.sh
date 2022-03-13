#!/bin/bash
./RplogParser --mode template2jsonDIR --in '/home/legacy/programok/core_stable/template/Template:RPG Post' --out '/home/legacy/programok/core_stable/template/users.json'
./RplogParser --mode textlog2jsonDIR --in '/home/legacy/programok/core_stable/template/posts' --out '/home/legacy/programok/core_stable/template/processed'
