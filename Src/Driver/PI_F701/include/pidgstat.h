/*-----------------------------------------------------------------------------

$Archive: /PI/clients/piapi/pidgstat.h $

-------------------------------------------------------------------------------

Copyright (c) 1996-2001 OSI Software, Inc. All rights reserved. 
Unpublished - rights reserved under the copyright law of the United States.

USE OF A COPYRIGHT NOTICE IS PRECAUTIONARY ONLY AND DOES NOT IMPLY PUBLICATION 
OR DISCLOSURE.

THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE SECRETS OF 
OSI SOFTWARE, INC.  USE, DISCLOSURE, OR REPRODUCTION IS PROHIBITED WITHOUT 
THE PRIOR EXPRESS WRITTEN PERMISSION OF OSI SOFTWARE, INC.

RESTRICTED RIGHTS LEGEND
Use, duplication, or disclosure by the Government is subject to restrictions 
as set forth in subparagraph (c)(1)(ii) of the Rights in Technical Data and 
Computer Software clause at DFARS 252.227.7013

OSI Software, Inc.
777 Davis Street, Suite 250, San Leandro CA 94577

-------------------------------------------------------------------------------

Description:
This file contains PI Digital States for interfaces

-------------------------------------------------------------------------------

$Log: /PI/clients/piapi/pidgstat.h $

5     8/31/01 9:38a Ray
Reformatted 3 new states.

4     1/10/01 2:53p Julie
Added 3 new digital states to system state set for use by OPC interface

#define OUTOFSERVICE	-312
#define COMMFAILURE         -313
#define NOTCONNECTED	-314

3     3/24/00 8:37a Charlie
Added INTFSHUTDOWN = -311.

2     1/03/00 4:35p Charlie
Added ACTIVEBATCHSTAT, WRONGTYPESTAT, OVERFLOW_ST
-------------------------------------------------------------------------------

@(#)pidgstat.h	1.1 10/16/96

History:

16-Oct-96  hs  Adopted from pidigstat.h from VMS.

-----------------------------------------------------------------------------*/ 

#define NOALARMSTAT                      -193
#define HIGHALARMSTAT                    -194
#define LOWALARMSTAT                     -195
#define HIALARM_ACKSTAT                  -196
#define LOALARM_ACKSTAT                  -197
#define NOALRM_UNACKSTAT                 -198
#define BADQUALITYSTAT                   -199
#define RATEALARMSTAT                    -200
#define RATEALM_ACKSTAT                  -201
#define DIGALARMSTAT                     -202
#define DIGALM_ACKSTAT                   -203
#define ACCESSDENIEDSTAT                 -210
#define NOSAMPLESTAT                     -211
#define NORESULTSTAT                     -212
#define UNITDOWNSTAT                     -213
#define SAMPLEBADSTAT                    -214
#define EQUIPFAILSTAT                    -215
#define NOLABDATASTAT                    -216
#define TRACESTAT                        -217
#define GREATERMMSTAT                    -218
#define BADLABDATASTAT                   -219
#define GOOD_OFFSTAT                     -220
#define GOOD_ONSTAT                      -221
#define ALARM_OFFSTAT                    -222
#define ALARM_ONSTAT                     -223
#define BADQUALITY_ST                    -224
#define BADQ_ONSTAT                      -225
#define BADQ_ALRM_OFSTAT                 -226
#define BADQ_ALRM_ONSTAT                 -227
#define MANUALSTAT                       -230
#define AUTOSTAT                         -231
#define CASC_RATIOSTAT                   -232
#define DCSFAILEDSTAT                    -233
#define MANUALLOCKSTAT                   -234
#define COBYPASSEDSTAT                   -235
#define BADOUTPUTSTAT                    -237
#define SCANOFFSTAT                      -238
#define SCANONSTAT                       -239
#define CONFIGURESTAT                    -240
#define FAILEDSTAT                       -241
#define ERRORSTAT                        -242
#define EXECUTESTAT                      -243
#define FILTERFAILSTAT                   -244
#define CALCOFFSTAT                      -245
#define INTFACESTOPSTAT                  -246
#define SETTOBADSTAT                     -247
#define NODATASTAT                       -248
#define CALCFAILEDSTAT                   -249
#define CALCOVERFLWSTAT                  -250
#define UNDERRANGESTAT                   -251
#define OVERRANGESTAT                    -252
#define PTCREATEDSTAT                    -253
#define SYSSHUTDOWNSTAT                  -254
#define BADINPUTSTAT                     -255
#define BADTOTALSTAT                     -256
#define NOALARM_ST                       -257
#define OVERUCLSTAT                      -258
#define UNDERLCLSTAT                     -259
#define OVERWLSTAT                       -260
#define UNDERWLSTAT                      -261
#define OVER1SIGMASTAT                   -262
#define UNDER1SIGMASTAT                  -263
#define OVERCENTERSTAT                   -264
#define UNDERCENTERSTAT                  -265
#define STRATIFIEDSTAT                   -266
#define MIXTURESTAT                      -267
#define TRENDUPSTAT                      -268
#define TRENDDOWNSTAT                    -269
#define NOALARM_STAT                     -270
#define OVERUCL_STAT                     -271
#define UNDERLCL_STAT                    -272
#define OVERWL_STAT                      -273
#define UNDERWL_STAT                     -274
#define OVER1SIGMA_STAT                  -275
#define UNDER1SIGM_STAT                  -276
#define OVERCENTER_STAT                  -277
#define UNDERCENTR_STAT                  -278
#define STRATIFIED_STAT                  -279
#define MIXTURE_STAT                     -280
#define TRENDUP_STAT                     -281
#define TRENDDOWN_STAT                   -282
#define ACTIVEBATCHSTAT                  -289
#define BADDATASTAT                      -290
#define CALCCRASHSTAT                    -291
#define CALCTIMEOUTSTAT                  -292
#define BADNARGSTAT                      -293
#define INPOUTRANGESTAT                  -294
#define NOTCONVERGESTAT                  -295
#define DSTFORWARDSTAT                   -296
#define DSTBACKSTAT                      -297
#define SUBSTITUTEDSTAT                  -298
#define INVALIDDATASTAT                  -299
#define SCANTIMEOUTSTAT                  -300
#define NOSAMPLE_ST                      -301
#define OFFLINESTAT                      -302
#define GOODSTAT                         -305
#define SUBSTITUTED_ST                   -306
#define BADSTAT                          -307
#define DOUBTFULSTAT                     -308
#define WRONGTYPESTAT                    -309
#define OVERFLOW_ST                      -310
#define INTFSHUTDOWN                     -311
#define OUTOFSERVICE                     -312
#define COMMFAILURE                      -313
#define NOTCONNECTED                     -314
