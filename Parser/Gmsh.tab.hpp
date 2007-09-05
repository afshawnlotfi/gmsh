typedef union {
  char *c;
  int i;
  unsigned int u;
  double d;
  double v[5];
  Shape s;
  List_T *l;
} YYSTYPE;
#define	tDOUBLE	257
#define	tSTRING	258
#define	tBIGSTR	259
#define	tEND	260
#define	tAFFECT	261
#define	tDOTS	262
#define	tPi	263
#define	tMPI_Rank	264
#define	tMPI_Size	265
#define	tEuclidian	266
#define	tCoordinates	267
#define	tExp	268
#define	tLog	269
#define	tLog10	270
#define	tSqrt	271
#define	tSin	272
#define	tAsin	273
#define	tCos	274
#define	tAcos	275
#define	tTan	276
#define	tRand	277
#define	tAtan	278
#define	tAtan2	279
#define	tSinh	280
#define	tCosh	281
#define	tTanh	282
#define	tFabs	283
#define	tFloor	284
#define	tCeil	285
#define	tFmod	286
#define	tModulo	287
#define	tHypot	288
#define	tPrintf	289
#define	tSprintf	290
#define	tStrCat	291
#define	tStrPrefix	292
#define	tStrRelative	293
#define	tBoundingBox	294
#define	tDraw	295
#define	tToday	296
#define	tPoint	297
#define	tCircle	298
#define	tEllipse	299
#define	tLine	300
#define	tSphere	301
#define	tPolarSphere	302
#define	tSurface	303
#define	tSpline	304
#define	tVolume	305
#define	tCharacteristic	306
#define	tLength	307
#define	tParametric	308
#define	tElliptic	309
#define	tPlane	310
#define	tRuled	311
#define	tTransfinite	312
#define	tComplex	313
#define	tPhysical	314
#define	tUsing	315
#define	tBump	316
#define	tProgression	317
#define	tPlugin	318
#define	tRotate	319
#define	tTranslate	320
#define	tSymmetry	321
#define	tDilate	322
#define	tExtrude	323
#define	tDuplicata	324
#define	tLoop	325
#define	tRecombine	326
#define	tDelete	327
#define	tCoherence	328
#define	tIntersect	329
#define	tBoundary	330
#define	tAttractor	331
#define	tLayers	332
#define	tHole	333
#define	tAlias	334
#define	tAliasWithOptions	335
#define	tText2D	336
#define	tText3D	337
#define	tInterpolationScheme	338
#define	tTime	339
#define	tGrain	340
#define	tCombine	341
#define	tBSpline	342
#define	tBezier	343
#define	tNurbs	344
#define	tOrder	345
#define	tKnots	346
#define	tColor	347
#define	tColorTable	348
#define	tFor	349
#define	tIn	350
#define	tEndFor	351
#define	tIf	352
#define	tEndIf	353
#define	tExit	354
#define	tField	355
#define	tThreshold	356
#define	tStructured	357
#define	tLatLon	358
#define	tGrad	359
#define	tPostView	360
#define	tReturn	361
#define	tCall	362
#define	tFunction	363
#define	tShow	364
#define	tHide	365
#define	tGetValue	366
#define	tGMSH_MAJOR_VERSION	367
#define	tGMSH_MINOR_VERSION	368
#define	tGMSH_PATCH_VERSION	369
#define	tAFFECTPLUS	370
#define	tAFFECTMINUS	371
#define	tAFFECTTIMES	372
#define	tAFFECTDIVIDE	373
#define	tOR	374
#define	tAND	375
#define	tEQUAL	376
#define	tNOTEQUAL	377
#define	tLESSOREQUAL	378
#define	tGREATEROREQUAL	379
#define	tPLUSPLUS	380
#define	tMINUSMINUS	381
#define	UNARYPREC	382


extern YYSTYPE yylval;
