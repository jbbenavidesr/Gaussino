#!/bin/bash
python $MUONMONISIMROOT/scripts/runmuontest.py EmStd
python $MUONMONISIMROOT/scripts/runmuontest.py EmOpt1
python $MUONMONISIMROOT/scripts/runmuontest.py EmOpt2
python $MUONMONISIMROOT/scripts/runmuontest.py EmOpt3
python $MUONMONISIMROOT/scripts/runmuontest.py EmNoCuts
python $MUONMONISIMROOT/scripts/runmuontest.py EmLHCb
python $MUONMONISIMROOT/scripts/runmuontest.py EmLHCbNoCuts

