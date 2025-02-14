#!/bin/bash
###############################################################################
# (c) Copyright 2025 CERN for the benefit of the LHCb and FCC Collaborations  #
#                                                                             #
# This software is distributed under the terms of the Apache License          #
# version 2 (Apache-2.0), copied verbatim in the file "COPYING".              #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################
. /cvmfs/lhcb.cern.ch/lib/LbEnv.sh
set -euo pipefail
uv -n venv test_env
source test_env/bin/activate
uv -n pip install myst_parser sphinx_rtd_theme
export PYTHONPATH_BASE=$(python -c "import sys; print(':'.join(sys.path))")
echo $PYTHONPATH_BASE
# -> adding python directories of the upstream projects
echo "Obtaining PYTHONPATH of the upstream projects for NIGHTLY_SLOT=$NIGHTLY_SLOT and NIGHTLY_PLATFORM=$NIGHTLY_PLATFORM"
export PYTHONPATH_PROJECTS=$(lb-run --nightly $NIGHTLY_SLOT -c $NIGHTLY_PLATFORM Gaussino/HEAD printenv PYTHONPATH)
echo $PYTHONPATH_PROJECTS
test -n "$PYTHONPATH_PROJECTS" || exit 2
# -> adding python directories of Gaussino
export PYTHONPATH_GAUSSINO=$(find $PWD -type d \( -path ./InstallArea -o -path ./build \) -prune -false -o -name "python" -print | paste -sd ":" - )
echo $PYTHONPATH_GAUSSINO
test -n "$PYTHONPATH_GAUSSINO" || exit 2
# -> building the docs with a custom pythonpath
PYTHONPATH=$PYTHONPATH_GAUSSINO:$PYTHONPATH_BASE:$PYTHONPATH_PROJECTS make -C docs html
test -f docs/_build/html/index.html || exit 2
