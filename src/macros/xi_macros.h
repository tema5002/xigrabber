#ifndef XI_HEADER_SIZE
#define XI_HEADER_SIZE (\
    96 + /* sample number for each note    */\
    48 + /* volume envelopes               */\
    48 + /* volume envelopes               */\
    1 +  /* volume envelope points amount  */\
    1 +  /* panning envelope points amount */\
    1 +  /* volume sustain point           */\
    1 +  /* volume loop start point        */\
    1 +  /* volume loop end point          */\
    1 +  /* panning sutain point           */\
    1 +  /* panning loop start point       */\
    1 +  /* panning loop end point         */\
    1 +  /* volume type                    */\
    1 +  /* panning type                   */\
    1 +  /* vibtype                        */\
    1 +  /* vibsweep                       */\
    1 +  /* vibdepth                       */\
    1 +  /* vibspeed                       */\
    2 +  /* fadeout                        */\
    22   /* reserved                       */)

#define XI_SAMPLE_HEADER_SIZE (\
    4 +  /* sample size        */\
    4 +  /* sample loop start  */\
    4 +  /* sample loop length */\
    1 +  /* volume             */\
    1 +  /* finetune           */\
    1 +  /* flags              */\
    1 +  /* panning            */\
    1 +  /* relative note      */\
    1 +  /* reserved           */\
    22   /* name               */)
#endif