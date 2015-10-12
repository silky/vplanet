/********************** ATMESC.C **********************/
/*
 * Rodrigo Luger, Tue May 12 11:11:38 PDT 2015
 *
 * Subroutines that control the integration of the 
 * atmospheric escape model.
 *
*/
// NOTE: Still need to edit module.c, update.c

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "vplanet.h"

void  InitializeControlAtmEsc(CONTROL *control) {
  /* Nothing for now, but this subroutine is necessary for module loops. */
}

void BodyCopyAtmEsc(BODY *dest,BODY *src,int foo,int iBody) {
  dest[iBody].dSurfaceWaterMass = src[iBody].dSurfaceWaterMass;
  dest[iBody].dXFrac = src[iBody].dXFrac;
  dest[iBody].dAtmXAbsEff = src[iBody].dAtmXAbsEff;
  dest[iBody].dMinSurfaceWaterMass = src[iBody].dMinSurfaceWaterMass;
}

void InitializeBodyAtmEsc(BODY *body,CONTROL *control,UPDATE *update,int iBody,int iModule) {
  //
}

void InitializeUpdateTmpBodyAtmEsc(BODY *body,CONTROL *control,UPDATE *update,int iBody) {
  //
}

/**************** ATMESC options ********************/

void ReadXFrac(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0) {
      if (control->Io.iVerbose >= VERBERR)
	      fprintf(stderr,"ERROR: %s must be >= 0.\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);	
    }
    body[iFile-1].dXFrac = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    if (iFile > 0)
      body[iFile-1].dXFrac = options->dDefault;
}


void ReadAtmXAbsEff(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0) {
      if (control->Io.iVerbose >= VERBERR)
	      fprintf(stderr,"ERROR: %s must be >= 0.\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);	
    }
    body[iFile-1].dAtmXAbsEff = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    if (iFile > 0)
      body[iFile-1].dAtmXAbsEff = options->dDefault;
}


void ReadSurfaceWaterMass(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].dSurfaceWaterMass = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    else
      body[iFile-1].dSurfaceWaterMass = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dSurfaceWaterMass = options->dDefault;
}

/* Halts */

void ReadHaltMinSurfaceWaterMass(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    control->Halt[iFile-1].bSurfaceDesiccated = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else {
    if (iFile > 0)
      AssignDefaultInt(options,&control->Halt[iFile-1].bSurfaceDesiccated,files->iNumInputs); 
  }
}

void ReadMinSurfaceWaterMass(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].dMinSurfaceWaterMass = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    else
      body[iFile-1].dMinSurfaceWaterMass = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dMinSurfaceWaterMass = options->dDefault;
}

void InitializeOptionsAtmEsc(OPTIONS *options,fnReadOption fnRead[]) {
  int iOpt,iFile;

  sprintf(options[OPT_XFRAC].cName,"dXFrac");
  sprintf(options[OPT_XFRAC].cDescr,"Fraction of planet radius in X-ray/XUV");
  sprintf(options[OPT_XFRAC].cDefault,"1");
  options[OPT_XFRAC].dDefault = 1;
  options[OPT_XFRAC].iType = 2;
  options[OPT_XFRAC].iMultiFile = 1;
  fnRead[OPT_XFRAC] = &ReadXFrac;
  
  sprintf(options[OPT_ATMXABSEFF].cName,"dAtmXAbsEff");
  sprintf(options[OPT_ATMXABSEFF].cDescr,"X-ray/XUV absorption efficiency (epsilon)");
  sprintf(options[OPT_ATMXABSEFF].cDefault,"0.15");
  options[OPT_ATMXABSEFF].dDefault = 0.15;
  options[OPT_ATMXABSEFF].iType = 2;
  options[OPT_ATMXABSEFF].iMultiFile = 1;
  fnRead[OPT_ATMXABSEFF] = &ReadAtmXAbsEff;

  sprintf(options[OPT_SURFACEWATERMASS].cName,"dSurfWaterMass");
  sprintf(options[OPT_SURFACEWATERMASS].cDescr,"Initial Surface Water Mass");
  sprintf(options[OPT_SURFACEWATERMASS].cDefault,"0");
  options[OPT_SURFACEWATERMASS].dDefault = 0;
  options[OPT_SURFACEWATERMASS].iType = 2;
  options[OPT_SURFACEWATERMASS].iMultiFile = 1;
  options[OPT_SURFACEWATERMASS].dNeg = TOMASS;
  sprintf(options[OPT_SURFACEWATERMASS].cNeg,"Terrestrial Oceans (TO)");
  fnRead[OPT_SURFACEWATERMASS] = &ReadSurfaceWaterMass;

  sprintf(options[OPT_HALTDESICCATED].cName,"bHaltSurfaceDesiccated");
  sprintf(options[OPT_HALTDESICCATED].cDescr,"Halt at Desiccation?");
  sprintf(options[OPT_HALTDESICCATED].cDefault,"0");
  options[OPT_HALTDESICCATED].iType = 0;
  fnRead[OPT_HALTDESICCATED] = &ReadHaltMinSurfaceWaterMass;

  sprintf(options[OPT_MINSURFACEWATERMASS].cName,"dMinSurfWaterMass");
  sprintf(options[OPT_MINSURFACEWATERMASS].cDescr,"Minimum Surface Water Mass");
  sprintf(options[OPT_MINSURFACEWATERMASS].cDefault,"1.e-5 TO");
  options[OPT_MINSURFACEWATERMASS].dDefault = 1.e-5*TOMASS;
  options[OPT_MINSURFACEWATERMASS].iType = 2;
  options[OPT_MINSURFACEWATERMASS].dNeg = TOMASS;
  sprintf(options[OPT_MINSURFACEWATERMASS].cNeg,"Terrestrial Oceans (TO)");
  fnRead[OPT_MINSURFACEWATERMASS] = &ReadMinSurfaceWaterMass;

}

void ReadOptionsAtmEsc(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,fnReadOption fnRead[],int iBody) {
  int iOpt;

  for (iOpt=OPTSTARTATMESC;iOpt<OPTENDATMESC;iOpt++) {
    if (options[iOpt].iType != -1) 
      fnRead[iOpt](body,control,files,&options[iOpt],system,iBody+1);
  }
}
    
/******************* Verify ATMESC ******************/

void VerifyRotationAtmEsc(BODY *body,CONTROL *control,OPTIONS *options,char cFile[],int iBody) {
  /* Nothing */
}

void VerifySurfaceWaterMass(BODY *body,OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {

  update[iBody].iaType[update[iBody].iSurfaceWaterMass][0] = 1;
  update[iBody].iNumBodies[update[iBody].iSurfaceWaterMass][0] = 1;
  update[iBody].iaBody[update[iBody].iSurfaceWaterMass][0] = malloc(update[iBody].iNumBodies[update[iBody].iSurfaceWaterMass][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].iSurfaceWaterMass][0][0] = iBody;

  update[iBody].pdDSurfaceWaterMassDtAtmesc = &update[iBody].daDerivProc[update[iBody].iSurfaceWaterMass][0];
  fnUpdate[iBody][update[iBody].iSurfaceWaterMass][0] = &fdDSurfaceWaterMassDt;
}

void fnPropertiesAtmEsc(BODY *body, UPDATE *update, int iBody) {
  /* Nothing */
}

void fnForceBehaviorAtmEsc(BODY *body,EVOLVE *evolve,IO *io,int iBody,int iModule) {
  
  if (body[iBody].dSurfaceWaterMass <= body[iBody].dMinSurfaceWaterMass)
    // Let's desiccate this planet.
    body[iBody].dSurfaceWaterMass = 0;
}

void VerifyAtmEsc(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  int bAtmEsc=0;

  /* AtmEsc is active for this body if this subroutine is called. */
  
  if (body[iBody].dSurfaceWaterMass > 0) {
    VerifySurfaceWaterMass(body,options,update,body[iBody].dAge,fnUpdate,iBody);
    bAtmEsc = 1;
  }
  
  if (!bAtmEsc && control->Io.iVerbose >= VERBINPUT) 
    fprintf(stderr,"WARNING: ATMESC called for body %s, but no atmosphere/water present!\n",body[iBody].cName);

  control->fnForceBehavior[iBody][iModule] = &fnForceBehaviorAtmEsc;
  control->Evolve.fnPropsAux[iBody][iModule] = &fnPropertiesAtmEsc;
  control->Evolve.fnBodyCopy[iBody][iModule] = &BodyCopyAtmEsc;

}

void InitializeModuleAtmEsc(CONTROL *control,MODULE *module) {
  /* Anything Here? */
}

/**************** ATMESC update ****************/

void InitializeUpdateAtmEsc(BODY *body,UPDATE *update,int iBody) {  
  if (body[iBody].dSurfaceWaterMass > 0) {
    if (update[iBody].iNumSurfaceWaterMass == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumSurfaceWaterMass++;
  }
}

void FinalizeUpdateEccAtmEsc(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  /* Nothing */
}

void FinalizeUpdateSurfaceWaterMassAtmEsc(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody) {
  update[iBody].iaModule[iVar][*iEqn] = ATMESC;
  update[iBody].iNumSurfaceWaterMass = (*iEqn)++;
}
      
void FinalizeUpdateOblAtmEsc(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  /* Nothing */
}

void FinalizeUpdateRotAtmEsc(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  /* Nothing */
}

void FinalizeUpdateSemiAtmEsc(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  /* Nothing */
}


/***************** ATMESC Halts *****************/

int fbHaltSurfaceDesiccated(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody) {
  
  if (body[iBody].dSurfaceWaterMass <= body[iBody].dMinSurfaceWaterMass) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's surface water mass =  ",body[iBody].cName);
      fprintd(stdout,body[iBody].dSurfaceWaterMass/TOMASS,io->iSciNot,io->iDigits);
      printf("TO.\n");
    }
    return 1;
  }
  return 0;
} 

void CountHaltsAtmEsc(HALT *halt,int *iHalt) {
  if (halt->bSurfaceDesiccated)
    (*iHalt)++;
}

void VerifyHaltAtmEsc(BODY *body,CONTROL *control,OPTIONS *options,int iBody,int *iHalt) {

  if (control->Halt[iBody].bSurfaceDesiccated)
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltSurfaceDesiccated;
}

/************* ATMESC Outputs ******************/

void HelpOutputAtmEsc(OUTPUT *output) {
  int iOut;

  printf("\n ------ ATMESC output ------\n");
  for (iOut=OUTSTARTATMESC;iOut<OUTENDATMESC;iOut++) 
    WriteHelpOutput(&output[iOut]);
}

void WriteSurfaceWaterMass(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dSurfaceWaterMass;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass,cUnit);
  }

}

void InitializeOutputAtmEsc(OUTPUT *output,fnWriteOutput fnWrite[]) {
  
  sprintf(output[OUT_SURFACEWATERMASS].cName,"SurfWaterMass");
  sprintf(output[OUT_SURFACEWATERMASS].cDescr,"Surface Water Mass");
  sprintf(output[OUT_SURFACEWATERMASS].cNeg,"TO");
  output[OUT_SURFACEWATERMASS].bNeg = 1;
  output[OUT_SURFACEWATERMASS].dNeg = 1./TOMASS;
  output[OUT_SURFACEWATERMASS].iNum = 1;
  fnWrite[OUT_SURFACEWATERMASS] = &WriteSurfaceWaterMass;

}

void FinalizeOutputFunctionAtmEsc(OUTPUT *output,int iBody,int iModule) {
  output[OUT_SURFENFLUX].fnOutput[iBody][iModule] = &fdSurfEnFluxAtmEsc;
}

/************ ATMESC Logging Functions **************/

void LogOptionsAtmEsc(CONTROL *control, FILE *fp) {

  /* Anything here?

  fprintf(fp,"-------- ATMESC Options -----\n\n");
  */
}

void LogAtmEsc(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp) {

  /* Anything here? 
  int iOut;

  fprintf(fp,"\n----- ATMESC PARAMETERS ------\n");
  for (iOut=OUTSTARTATMESC;iOut<OUTBODYSTARTATMESC;iOut++) {
    if (output[iOut].iNum > 0)
      WriteLogEntry(control,output[iOut],body,system,fnWrite[iOut],fp,update,0);
  }
  */
}

void LogBodyAtmEsc(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp,int iBody) {
  int iOut;  
  fprintf(fp,"----- ATMESC PARAMETERS (%s)------\n",body[iBody].cName);
  
  for (iOut=OUTSTARTATMESC;iOut<OUTENDATMESC;iOut++) {
    if (output[iOut].iNum > 0) 
      WriteLogEntry(body,control,&output[iOut],system,update,fnWrite[iOut],fp,iBody);
    /*
    fprintf(fp,"40K Constant: ");
    fprintd(fp,body[iBody].d40KConst,control->Io.iSciNot,control->Io.iDigits);
    fprintf(fp,"\n");

    fprintf(fp,"232Th Constant: ");
    fprintd(fp,body[iBody].d232ThConst,control->Io.iSciNot,control->Io.iDigits);
    fprintf(fp,"\n");

    fprintf(fp,"238U Constant: ");
    fprintd(fp,body[iBody].d238UConst,control->Io.iSciNot,control->Io.iDigits);
    fprintf(fp,"\n");
    */
  }
}

void AddModuleAtmEsc(MODULE *module,int iBody,int iModule) {

  module->iaModule[iBody][iModule] = ATMESC;

  module->fnInitializeControl[iBody][iModule] = &InitializeControlAtmEsc;
  module->fnInitializeUpdateTmpBody[iBody][iModule] = &InitializeUpdateTmpBodyAtmEsc;

  module->fnCountHalts[iBody][iModule] = &CountHaltsAtmEsc;
  module->fnReadOptions[iBody][iModule] = &ReadOptionsAtmEsc;
  module->fnLogBody[iBody][iModule] = &LogBodyAtmEsc;
  module->fnVerify[iBody][iModule] = &VerifyAtmEsc;
  module->fnVerifyHalt[iBody][iModule] = &VerifyHaltAtmEsc;
  module->fnVerifyRotation[iBody][iModule] = &VerifyRotationAtmEsc;

  module->fnInitializeBody[iBody][iModule] = &InitializeBodyAtmEsc;
  module->fnInitializeUpdate[iBody][iModule] = &InitializeUpdateAtmEsc;
  module->fnFinalizeUpdateSurfaceWaterMass[iBody][iModule] = &FinalizeUpdateSurfaceWaterMassAtmEsc;

  //module->fnIntializeOutputFunction[iBody][iModule] = &InitializeOutputFunctionAtmEsc;
  module->fnFinalizeOutputFunction[iBody][iModule] = &FinalizeOutputFunctionAtmEsc;

}

/************* ATMESC Functions ************/

double fdDSurfaceWaterMassDt(BODY *body,SYSTEM *system,int *iaBody) {
  // TODO: Currently this is just Erkaev's model. Add other escape regimes
  
  double elim, fxuv, xi, ktide;
  
  xi = (pow(body[iaBody[0]].dMass / (3. * body[0].dMass), (1. / 3)) * 
       body[iaBody[0]].dSemi) / (body[iaBody[0]].dRadius * body[iaBody[0]].dXFrac);
  if (xi > 1)	ktide = (1 - 3 / (2 * xi) + 1 / (2 * pow(xi, 3)));
	else ktide = 0;

  fxuv = body[0].dLXUV / (4 * PI * pow(body[iaBody[0]].dSemi, 2) * 
         pow((1 - body[iaBody[0]].dEcc * body[iaBody[0]].dEcc), 0.5));
  
  elim = PI * pow(body[iaBody[0]].dRadius, 3) * pow(body[iaBody[0]].dXFrac, 2) * 
         body[iaBody[0]].dAtmXAbsEff * fxuv / (BIGG * body[iaBody[0]].dMass * ktide);

  //printf(body[iaBody[0]].dSurfaceWaterMass)

  return -elim;
}

double fdSurfEnFluxAtmEsc(BODY *body,SYSTEM *system,UPDATE *update,int iBody,int iFoo) {
  // This is silly, but necessary!
  return 0;
}