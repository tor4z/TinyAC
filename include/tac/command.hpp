#pragma once


namespace tac
{

class Arguments;


void processNoneSubCommand(Arguments &arg);
void usage();
void version();


void processRunSubCommand(Arguments &arg);
void processExecSubCommand(Arguments &arg);
void processCommitSubCommand(Arguments &arg);
void processBuildSubCommand(Arguments &arg);
void processRemoveSubCommand(Arguments &arg);
void processKillSubCommand(Arguments &arg);
void processListSubCommand(Arguments &arg);
void processRemoveImgSubCommand(Arguments &arg);
void processImagesSubCommand(Arguments &arg);



}
