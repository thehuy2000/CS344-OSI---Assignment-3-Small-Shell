#!/bin/bash

echo "PRE-SCRIPT INFO"
echo "  Grading Script PID: $$"
echo '  Note: your smallsh will report a different PID when evaluating $$'

./smallsh <<'___EOF___'
echo BEGINNING TEST SCRIPT
echo
echo --------------------
echo Using comment (5 points if only next prompt is displayed next)
#THIS COMMENT SHOULD DO NOTHING
echo
echo
echo --------------------
echo ls (10 points for returning dir contents)
ls
echo
echo
echo --------------------
echo ls out junk
ls > junk
echo
echo
echo --------------------
echo cat junk (15 points for correctly returning contents of junk)
cat junk
echo
echo
echo --------------------
echo wc in junk (15 points for returning correct numbers from wc)
wc < junk
echo
echo
echo --------------------
echo wc in junk out junk2; cat junk2 (10 points for returning correct numbers from wc)
wc < junk > junk2
cat junk2
echo
echo
echo --------------------
echo test -f badfile (10 points for returning error value of 1, note extraneous &)
test -f badfile
status &
echo
echo
echo --------------------
echo wc in badfile (10 points for returning text error)
wc < badfile
echo
echo
echo --------------------
echo badfile (10 points for returning text error)
badfile
echo
echo
echo --------------------
echo sleep 100 background (10 points for returning process ID of sleeper)
sleep 100 &
echo
echo
echo --------------------
echo pkill -signal SIGTERM sleep (10 points for pid of killed process, 10 points for signal)
echo (Ignore message about Operation Not Permitted)
pkill sleep
echo
echo
echo --------------------
echo sleep 1 background (10 pts for pid of bg ps when done, 10 for exit value)
sleep 1 &
sleep 1
echo
echo
echo --------------------
echo pwd
pwd
echo
echo
echo --------------------
echo cd
cd
echo
echo
echo --------------------
echo pwd (10 points for being in the HOME dir)
pwd
echo
echo
echo --------------------
echo mkdir testdir$$
mkdir testdir$$
echo
echo
echo --------------------
echo cd testdir$$
cd testdir$$
echo
echo
echo --------------------
echo pwd (5 points for being in the newly created dir)
pwd
echo --------------------
echo Testing foreground-only mode (20 points for entry & exit text AND ~5 seconds between times)
kill -SIGTSTP $$
date
sleep 5 &
date
kill -SIGTSTP $$
exit
___EOF___
