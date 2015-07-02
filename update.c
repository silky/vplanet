#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include "vplanet.h"

void InitializeUpdateBodyPerts(CONTROL *control,UPDATE *update,int iBody) {
  int iVar,iEqn;

  for (iVar=0;iVar<update[iBody].iNumVars;iVar++) {
    for (iEqn=0;iEqn<update[iBody].iNumEqns[iVar];iEqn++) {
      control->Evolve.tmpUpdate[iBody].iaBody[iVar][iEqn] = malloc(update[iBody].iNumBodies[iVar][iEqn]);
    }
  }
}

void InitializeUpdateTmpBody(BODY *body,CONTROL *control,MODULE *module,UPDATE *update,int iBody) {
  int iModule;

  for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
    module->fnInitializeUpdateTmpBody[iBody][iModule](body,control,update,iBody);
}

void UpdateCopy(UPDATE *dest,UPDATE *src,int iNumBodies) {
  int iBody,iVar,iEqn,iPert;

  for (iBody=0;iBody<iNumBodies;iBody++) {
    dest[iBody].iNumEcc = src[iBody].iNumEcc;
    dest[iBody].iEcc = src[iBody].iEcc;

    dest[iBody].iNum40KMan =src[iBody].iNum40KMan;
    dest[iBody].i40KMan =src[iBody].i40KMan;

    dest[iBody].iNum232ThMan =src[iBody].iNum232ThMan;
    dest[iBody].i232ThMan =src[iBody].i232ThMan;

    dest[iBody].iNum238UMan =src[iBody].iNum238UMan;
    dest[iBody].i238UMan =src[iBody].i238UMan;

    dest[iBody].iNum235UMan =src[iBody].iNum235UMan;
    dest[iBody].i235UMan =src[iBody].i235UMan;

    dest[iBody].iNumObl = src[iBody].iNumObl;
    dest[iBody].iObl = src[iBody].iObl;

    dest[iBody].iNumRot = src[iBody].iNumRot;
    dest[iBody].iRot = src[iBody].iRot;

    dest[iBody].iNumSemi = src[iBody].iNumSemi;
    dest[iBody].iSemi = src[iBody].iSemi;

    dest[iBody].iNumVars = src[iBody].iNumVars;

    for (iVar=0;iVar<src[iBody].iNumVars;iVar++) {
      dest[iBody].iNumEqns[iVar] = src[iBody].iNumEqns[iVar];
      dest[iBody].iaVar[iVar] = src[iBody].iaVar[iVar];

      for (iEqn=0;iEqn<src[iBody].iNumEqns[iVar];iEqn++) {
	dest[iBody].iNumBodies[iVar][iEqn] = src[iBody].iNumBodies[iVar][iEqn];
	dest[iBody].iaType[iVar][iEqn] = src[iBody].iaType[iVar][iEqn];
	dest[iBody].iaModule[iVar][iEqn] = src[iBody].iaModule[iVar][iEqn];
	for (iPert=0;iPert<src[iBody].iNumBodies[iVar][iEqn];iPert++)
	  dest[iBody].iaBody[iVar][iEqn][iPert] = src[iBody].iaBody[iVar][iEqn][iPert];
      }
    }
  }
}

void InitializeUpdate(BODY*body,CONTROL *control,MODULE *module,UPDATE *update,fnUpdateVariable ****fnUpdate) {
  int iBody,iBodyPert,iVar,iEqn,iModule;
  int iNum,iSubStep;

  // Allocate the firt dimension of the Update matrix to be the number of bodies
  *fnUpdate=malloc(control->Evolve.iNumBodies*sizeof(fnUpdateVariable**));

  // For each body, determine how many independent variables need updating.
  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
    update[iBody].iaModule = malloc(module->iNumModules[iBody]*sizeof(int));

    // Assume no modules to start
    update[iBody].iNumEcc=0;
    update[iBody].iNum40KMan=0;
    update[iBody].iNum232ThMan=0;
    update[iBody].iNum238UMan=0;
    update[iBody].iNum235UMan=0;
    update[iBody].iNum40KCore=0;
    update[iBody].iNum232ThCore=0;
    update[iBody].iNum238UCore=0;
    update[iBody].iNum235UCore=0;
    update[iBody].iNumObl=0;
    update[iBody].iNumRot=0;
    update[iBody].iNumSemi=0;
    update[iBody].iNumVars=0;
    
    /* First we must identify how many variables and models must be 
       assigned so we can malloc the update struct. */
    for (iModule=0;iModule<module->iNumModules[iBody];iModule++)
      module->fnInitializeUpdate[iBody][iModule](body,update,iBody);

    // The second dimension of the Update matrix is the variables
    (*fnUpdate)[iBody]=malloc(update[iBody].iNumVars*sizeof(fnUpdateVariable*));
    update[iBody].iaVar = malloc(update[iBody].iNumVars*sizeof(int));
    update[iBody].iNumEqns = malloc(update[iBody].iNumVars*sizeof(double));
    update[iBody].iaType = malloc(update[iBody].iNumVars*sizeof(int*));
    update[iBody].iaModule = malloc(update[iBody].iNumVars*sizeof(int*));
    update[iBody].pdVar = malloc(update[iBody].iNumVars*sizeof(double*));
    update[iBody].daDeriv = malloc(update[iBody].iNumVars*sizeof(double*));
    update[iBody].daDerivProc = malloc(update[iBody].iNumVars*sizeof(double*));
    update[iBody].iNumBodies = malloc(update[iBody].iNumVars*sizeof(int*));
    update[iBody].iaBody = malloc(update[iBody].iNumVars*sizeof(int**));

    // May also have to allocate space for the temp UPDATE
    if (control->Evolve.iOneStep == RUNGEKUTTA) {
      control->Evolve.tmpUpdate[iBody].iaVar = malloc(update[iBody].iNumVars*sizeof(int));
      control->Evolve.tmpUpdate[iBody].iNumEqns = malloc(update[iBody].iNumVars*sizeof(double));
      control->Evolve.tmpUpdate[iBody].iaType = malloc(update[iBody].iNumVars*sizeof(int*));
      control->Evolve.tmpUpdate[iBody].iaModule = malloc(update[iBody].iNumVars*sizeof(int*));
      control->Evolve.tmpUpdate[iBody].pdVar = malloc(update[iBody].iNumVars*sizeof(double*));
      control->Evolve.tmpUpdate[iBody].daDeriv = malloc(update[iBody].iNumVars*sizeof(double*));
      control->Evolve.tmpUpdate[iBody].daDerivProc = malloc(update[iBody].iNumVars*sizeof(double*));
      control->Evolve.tmpUpdate[iBody].iNumBodies = malloc(update[iBody].iNumVars*sizeof(int*));
      control->Evolve.tmpUpdate[iBody].iaBody = malloc(update[iBody].iNumVars*sizeof(int**));
    }
    for (iSubStep=0;iSubStep<4;iSubStep++) 
      control->Evolve.daDeriv[iSubStep][iBody]=malloc(update[iBody].iNumVars*sizeof(double));

    // Eccentricity

    iVar=0;
    update[iBody].iEcc = -1; // Assume not tracked
    if (update[iBody].iNumEcc) {
      // See vplanet.h for description of Update members
      update[iBody].iEcc = iVar;
      update[iBody].iaVar[iVar] = VECC;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumEcc;
      // XXX assign pointer out of if-then?
      update[iBody].pdVar[iVar] = &body[iBody].dEcc;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumEcc*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNumEcc*sizeof(int*));

      update[iBody].iaType[iVar] = malloc(update[iBody].iNumEcc*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNumEcc*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
	control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].dEcc;
	control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumEcc*sizeof(int));
	control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNumEcc*sizeof(int*));
	control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNumEcc*sizeof(double));
	control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNumEcc*sizeof(int));
	control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNumEcc*sizeof(int));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
	module->fnFinalizeUpdateEcc[iBody][iModule](body,update,&iEqn,iVar,iBody);
      
      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }

    // Radiogenic Isotopes
    /* 40K Mantle */
    update[iBody].i40KMan = -1;
    if (update[iBody].iNum40KMan) {
      update[iBody].i40KMan = iVar;
      update[iBody].iaVar[iVar] = VNUM40KMAN;
      update[iBody].iNumEqns[iVar] = update[iBody].iNum40KMan;
      update[iBody].pdVar[iVar] = &body[iBody].d40KNumMan;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNum40KMan*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNum40KMan*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNum40KMan*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNum40KMan*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
	control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].d40KNumMan;
	control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNum40KMan*sizeof(int));
	control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNum40KMan*sizeof(double));
	control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNum40KMan*sizeof(int));
	control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNum40KMan*sizeof(int));
	control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNum40KMan*sizeof(int*));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
	  module->fnFinalizeUpdate40KNumMan[iBody][iModule](body,update,&iEqn,iVar,iBody);
      
      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }
    /* 40K Core */
    update[iBody].i40KCore = -1;
    if (update[iBody].iNum40KCore) {
      update[iBody].i40KCore = iVar;
      update[iBody].iaVar[iVar] = VNUM40KCORE;
      update[iBody].iNumEqns[iVar] = update[iBody].iNum40KCore;
      update[iBody].pdVar[iVar] = &body[iBody].d40KNumCore;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNum40KCore*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNum40KCore*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNum40KCore*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNum40KCore*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
	control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].d40KNumCore;
	control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNum40KCore*sizeof(int));
	control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNum40KCore*sizeof(double));
	  control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNum40KCore*sizeof(int));
	  control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNum40KCore*sizeof(int));
	control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNum40KCore*sizeof(int*));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
	  module->fnFinalizeUpdate40KNumCore[iBody][iModule](body,update,&iEqn,iVar,iBody);
      
      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }

    /* 232Th Mantle */    
    update[iBody].i232ThMan = -1;
    if (update[iBody].iNum232ThMan) {
      update[iBody].i232ThMan = iVar;
      update[iBody].iaVar[iVar] = VNUM232THMAN;
      update[iBody].iNumEqns[iVar] = update[iBody].iNum232ThMan;
      update[iBody].pdVar[iVar] = &body[iBody].d232ThNumMan;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNum232ThMan*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNum232ThMan*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNum232ThMan*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNum232ThMan*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
	control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].d232ThNumMan;
	control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNum232ThMan*sizeof(int));
	control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNum232ThMan*sizeof(double));
	  control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNum232ThMan*sizeof(int));
	  control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNum232ThMan*sizeof(int));
	control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNum232ThMan*sizeof(int*));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
	module->fnFinalizeUpdate232ThNumMan[iBody][iModule](body,update,&iEqn,iVar,iBody);
      
      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }
    /* 232Th Core */    
    update[iBody].i232ThCore = -1;
    if (update[iBody].iNum232ThCore) {
      update[iBody].i232ThCore = iVar;
      update[iBody].iaVar[iVar] = VNUM232THCORE;
      update[iBody].iNumEqns[iVar] = update[iBody].iNum232ThCore;
      update[iBody].pdVar[iVar] = &body[iBody].d232ThNumCore;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNum232ThCore*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNum232ThCore*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNum232ThCore*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNum232ThCore*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
	control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].d232ThNumCore;
	control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNum232ThCore*sizeof(int));
	control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNum232ThCore*sizeof(double));
	  control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNum232ThCore*sizeof(int));
	  control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNum232ThCore*sizeof(int));
	control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNum232ThCore*sizeof(int*));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
	module->fnFinalizeUpdate232ThNumCore[iBody][iModule](body,update,&iEqn,iVar,iBody);
      
      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }

    /* 238U Mantle */    
    update[iBody].i238UMan = -1;
    if (update[iBody].iNum238UMan) {
      update[iBody].i238UMan = iVar;
      update[iBody].iaVar[iVar] = VNUM238UMAN;
      update[iBody].iNumEqns[iVar] = update[iBody].iNum238UMan;
      update[iBody].pdVar[iVar] = &body[iBody].d238UNumMan;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNum238UMan*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNum238UMan*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNum238UMan*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNum238UMan*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
	control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].d238UNumMan;
	control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNum238UMan*sizeof(int));
	control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNum238UMan*sizeof(double));
	  control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNum238UMan*sizeof(int));
	  control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNum238UMan*sizeof(int));
	control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNum238UMan*sizeof(int*));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
	module->fnFinalizeUpdate238UNumMan[iBody][iModule](body,update,&iEqn,iVar,iBody);
      
      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }
    /* 238U Core */    
    update[iBody].i238UCore = -1;
    if (update[iBody].iNum238UCore) {
      update[iBody].i238UCore = iVar;
      update[iBody].iaVar[iVar] = VNUM238UCORE;
      update[iBody].iNumEqns[iVar] = update[iBody].iNum238UCore;
      update[iBody].pdVar[iVar] = &body[iBody].d238UNumCore;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNum238UCore*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNum238UCore*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNum238UCore*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNum238UCore*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
	control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].d238UNumCore;
	control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNum238UCore*sizeof(int));
	control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNum238UCore*sizeof(double));
	  control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNum238UCore*sizeof(int));
	  control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNum238UCore*sizeof(int));
	control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNum238UCore*sizeof(int*));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
	module->fnFinalizeUpdate238UNumCore[iBody][iModule](body,update,&iEqn,iVar,iBody);
      
      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }
    
    /* 235U Mantle */            
    update[iBody].i235UMan = -1;  
    if (update[iBody].iNum235UMan) {
      update[iBody].i235UMan = iVar;
      update[iBody].iaVar[iVar] = VNUM235UMAN;
      update[iBody].iNumEqns[iVar] = update[iBody].iNum235UMan;
      update[iBody].pdVar[iVar] = &body[iBody].d235UNumMan;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNum235UMan*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNum235UMan*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNum235UMan*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNum235UMan*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
	control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].d235UNumMan;
	control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNum235UMan*sizeof(int));
	control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNum235UMan*sizeof(double));
	  control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNum235UMan*sizeof(int));
	  control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNum235UMan*sizeof(int));
	control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNum235UMan*sizeof(int*));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
	module->fnFinalizeUpdate235UNumMan[iBody][iModule](body,update,&iEqn,iVar,iBody);
      
      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }
    /* 235U Core */            
    update[iBody].i235UCore = -1;  
    if (update[iBody].iNum235UCore) {
      update[iBody].i235UCore = iVar;
      update[iBody].iaVar[iVar] = VNUM235UCORE;
      update[iBody].iNumEqns[iVar] = update[iBody].iNum235UCore;
      update[iBody].pdVar[iVar] = &body[iBody].d235UNumCore;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNum235UCore*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNum235UCore*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNum235UCore*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNum235UCore*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
	control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].d235UNumCore;
	control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNum235UCore*sizeof(int));
	control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNum235UCore*sizeof(double));
	  control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNum235UCore*sizeof(int));
	  control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNum235UCore*sizeof(int));
	control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNum235UCore*sizeof(int*));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
	module->fnFinalizeUpdate235UNumCore[iBody][iModule](body,update,&iEqn,iVar,iBody);
      
      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }
    
    // Obliquity
    
    update[iBody].iObl = -1;
    if (update[iBody].iNumObl) {
      update[iBody].iObl = iVar;
      update[iBody].iaVar[iVar] = VOBL;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumObl;
      update[iBody].pdVar[iVar] = &body[iBody].dObliquity;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumObl*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNumObl*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumObl*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNumObl*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
	control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].dObliquity;
	control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumObl*sizeof(int));
	control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNumObl*sizeof(double));
	control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNumObl*sizeof(int));
	control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNumObl*sizeof(int));
	control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNumObl*sizeof(int*));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
	module->fnFinalizeUpdateObl[iBody][iModule](body,update,&iEqn,iVar,iBody);
      
      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }
    
    // Rotation Rate

    update[iBody].iRot = -1;
    if (update[iBody].iNumRot) {
      update[iBody].iRot = iVar;
      update[iBody].iaVar[iVar] = VROT;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumRot;
      update[iBody].pdVar[iVar] = &body[iBody].dRotRate;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumRot*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNumRot*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumRot*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNumRot*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
	control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].dRotRate;
	control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumRot*sizeof(int));
	control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNumRot*sizeof(double));
	control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNumRot*sizeof(int));
	control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNumRot*sizeof(int));
	control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNumRot*sizeof(int*));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
	module->fnFinalizeUpdateRot[iBody][iModule](body,update,&iEqn,iVar,iBody);
      
      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }
    
    // Semi-major Axis

    update[iBody].iSemi = -1;
    if (update[iBody].iNumSemi) {
      update[iBody].iSemi = iVar;
      update[iBody].iaVar[iVar] = VSEMI;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumSemi;
      update[iBody].pdVar[iVar] = &body[iBody].dSemi;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumSemi*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNumSemi*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumSemi*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNumSemi*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
	control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].dSemi;
	control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumSemi*sizeof(int));
	control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNumSemi*sizeof(double));
	control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNumSemi*sizeof(int));
	control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNumSemi*sizeof(int));
	control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNumSemi*sizeof(int*));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
	module->fnFinalizeUpdateSemi[iBody][iModule](body,update,&iEqn,iVar,iBody);

      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }
  }
}