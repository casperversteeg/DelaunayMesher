#!/bin/bash

# Clean the directory
rm test/*.msh test/*.log

./mesh-generator test/*.txt
