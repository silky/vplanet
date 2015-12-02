/***************** POISE.H *********************** 
 *
 * Russell Deitrick, September 10, 2015
 *
 * This header file contains all the subroutines in
 * file poise.c.
 *
*/

void InitializeControlPoise(CONTROL*);
void AddModulePoise(MODULE*,int,int);
void BodyCopyPoise(BODY*,BODY*,int,int);
void InitializeBodyPoise(BODY*,CONTROL*,UPDATE*,int,int);
void InitializeUpdateTmpBodyPoise(BODY*,CONTROL*,UPDATE*,int);

/* Options Info */

#define OPTSTARTPOISE       1800 /* Start of POISE options */
#define OPTENDPOISE         1900 /* End of POISE options */

#define OPT_LATCELLNUM      1801
#define OPT_PLANCKA         1802
#define OPT_PLANCKB         1803
#define OPT_DIFFUSION       1804
#define OPT_HEATCAPANN      1805
#define OPT_HADLEY          1806
#define OPT_COLDSTART       1807
#define OPT_FIXICELAT       1808
#define OPT_ALBEDOZA        1809
#define OPT_JORMUNGAND      1810
#define OPT_CALCAB          1811
#define OPT_TGLOBALEST      1812
#define OPT_PCO2            1813
#define OPT_MEPDIFF         1814
#define OPT_ICESHEETS       1815
#define OPT_INITICELAT      1816
#define OPT_INITICEHEIGHT   1817
#define OPT_ICEALBEDO       1818
#define OPT_SURFALBEDO      1819

/*#define OPT_LANDGEOM        1840
#define OPT_ICEMODEL        1841
#define OPT_HEATCLAND       1842
#define OPT_HEATCWATER      1843
#define OPT_TFREEZE         1844
#define OPT_LATENTHEAT      1845
#define OPT_CONDUCTIVITY    1846
#define OPT_MLDEPTH         1847
#define OPT_NU              1848 

#define OPT_EBMMODEL        1899*/

/* Options Functions */
void HelpOptionsPoise(OPTIONS*);
void InitializeOptionsPoise(OPTIONS*,fnReadOption[]);
void ReadOptionsPoise(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,fnReadOption[],int);

/* Verify Functions */
void VerifyPoise(BODY*,CONTROL*,FILES*,OPTIONS*,OUTPUT*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,int);

/* Update Functions */

void InitializeUpdatePoise(BODY*,UPDATE*,int);

/* Output Functinos */

/* POISE 1800 - 1899 */
/* System properties 1800-1819, body properties 1820-1899 */

#define OUTSTARTPOISE        1800 /* Start of POISE options */
#define OUTENDPOISE          1900 /* End of POISE options */
#define OUTBODYSTARTPOISE    1820 /* Start of POISE BODY options */

#define OUT_TGLOBAL          1821 
#define OUT_ALBEDOGLOBAL     1822
#define OUT_FLUXINGLOBAL     1823
#define OUT_FLUXOUTGLOBAL    1824

#define OUT_LATITUDE         1830
#define OUT_TEMPLAT          1831
#define OUT_ALBEDOLAT        1832
#define OUT_ANNUALINSOL      1833
#define OUT_DAILYINSOL       1834
#define OUT_FLUXMERID        1835
#define OUT_FLUXIN           1836
#define OUT_FLUXOUT          1837
#define OUT_DIVFLUX          1838
#define OUT_ICEMASS          1839
#define OUT_ICEHEIGHT        1840
#define OUT_TOTICEMASS			 1841

void InitializeOptionsPoise(OPTIONS*,fnReadOption[]);
void FinalizeUpdateIceMassPoise(BODY*,UPDATE*,int*,int,int,int);


void HelpOutputPoise(OUTPUT*);
void WriteTGlobal(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteAlbedoGlobal(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteTempLat(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteAlbedoLat(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteAnnualInsol(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteDailyInsol(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteFluxMerid(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteFluxIn(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteFluxOut(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteDivFlux(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void InitializeOutputPoise(OUTPUT*,fnWriteOutput[]);

/* Logging Functions */
void LogOptionsPoise(CONTROL*,FILE*);
void LogPoise(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*);
void LogBodyPoise(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*,int);

/* Poise Functions */
void PropertiesPoise(BODY*,UPDATE*,int);
void ForceBehaviorPoise(BODY*,EVOLVE*,IO*,SYSTEM*,UPDATE*,int,int);
void Albedo(BODY*,int);
void AnnualInsolation(BODY*,int);
double dOLRdTwk97(BODY*,int);
double OLRwk97(BODY*,int);

void PoiseClimate(BODY*,int);
double fdPoiseDIceMassDt(BODY*,SYSTEM*,int*);
