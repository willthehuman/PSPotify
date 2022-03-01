rem Change working directory to where this batch file is stored
cd %~dp0

rem Create container, run make
docker run --name pspdev -v %cd%:/pspmake willthehuman/pspdev /bin/bash -c "export PSPDEV=/usr/local/pspdev && export PATH=$PATH:$PSPDEV/bin && cd pspmake && make"

rem Delete the container
docker rm pspdev

rem Move EBOOT.PBP to the game folder
copy EBOOT.pbp build\EBOOT.pbp

pause