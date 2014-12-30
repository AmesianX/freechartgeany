/*
 * function_dataset.cpp
 * 
 * Copyright 2013 Lucas Tsatiris <chartgeany@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#include <stdlib.h>
#include "ta_func.h"
#include "function_dataset.h"

// dummy: returns input 
DataSet 
DUMMY (DataSet dset, int period)
{
  PriceVector *result = NULL;	
  qint32 setsize;
  
  if (period > 0)
    period = 0;
      
  result = new PriceVector;
  setsize = dset->size ();
  
  for (qint32 counter = 0; counter < setsize; counter ++)
    result->append ((qreal) dset->at (counter));
  return result;
}

// simple moving average 
DataSet 
SMA (DataSet dset, int period)
{
  PriceVector *result = NULL;
  float *input;
  double *output = NULL;
  TA_RetCode retcode;
  int outBegIdx, outNbElement;
  qint32 setsize;

  if (period < 2)
    period = 2;
  
  setsize = dset->size ();
  input = (float *) calloc (setsize + 1, sizeof (float));
  if (input == NULL)
    return NULL;
  
  output = (double *) calloc (setsize + 1, sizeof (double));
  if (output == NULL)
  {
	free ((void *) input);  
    return NULL;  
  }  
  
  for (qint32 counter = setsize; counter > 0; counter --)
    input[setsize - counter] = dset->at (counter - 1);
  
  retcode = TA_S_MA(0, setsize - 1, input, period, TA_MAType_SMA, 
                    &outBegIdx, &outNbElement, output);
  result = new PriceVector;
  if (retcode == TA_SUCCESS)
    for (qint32 counter = outNbElement - 1; counter >=0; counter --)
      result->append ((qreal) output[counter]);
  
  free ((void *) input);
  free ((void *) output);                     
  return result;    	
}

// exponential moving average 
DataSet 
EMA (DataSet dset, int period)
{
  PriceVector *result = NULL;
  float *input;
  double *output = NULL;
  TA_RetCode retcode;
  int outBegIdx, outNbElement;
  qint32 setsize;
  
  if (period < 2)
    period = 2;
  
  setsize = dset->size ();
  input = (float *) calloc (setsize + 1, sizeof (float));
  if (input == NULL)
    return NULL;
  
  output = (double *) calloc (setsize + 1, sizeof (double));
  if (output == NULL)
  {
	free ((void *) input);  
    return NULL;  
  }  
  
  for (qint32 counter = setsize; counter > 0; counter --)
    input[setsize - counter] = dset->at (counter - 1);
  
  retcode = TA_S_MA(0, setsize - 1, input, period, TA_MAType_EMA, 
                    &outBegIdx, &outNbElement, output);
  
  result = new PriceVector;
  if (retcode == TA_SUCCESS)
    for (qint32 counter = outNbElement - 1; counter >=0; counter --)
      result->append ((qreal) output[counter]);
  
  free ((void *) input);
  free ((void *) output);                     
  return result;    	
}

// parabolic SAR
DataSet 
PSAR (FrameVector *HLOC, int period)
{
  PriceVector *result = NULL;
  QTAChartFrame frame;
  float *high, *low;
  double *output = NULL;
  TA_RetCode retcode;
  int outBegIdx, outNbElement;
  qint32 setsize;
  
  if (period < 2)
    period = 2;
  
  setsize = HLOC->size ();
  high = (float *) calloc (setsize + 1, sizeof (float));
  if (high == NULL)
    return NULL;
  
  low = (float *) calloc (setsize + 1, sizeof (float));
  if (low == NULL)
  {
	free ((void *) low);    
    return NULL;
  }  
  
  output = (double *) calloc (setsize + 1, sizeof (double));
  if (output == NULL)
  {
	free ((void *) low);    
	free ((void *) high);  
    return NULL;  
  }  

  for (qint32 counter = setsize; counter > 0; counter --)
  {
	frame = HLOC->at (counter - 1);
    high[setsize - counter] = frame.High;
    low[setsize - counter] =  frame.Low;
  }  

  retcode = TA_S_SAR (0, setsize - 1, high, low, 0.1, 0.1, 
                    &outBegIdx, &outNbElement, output);
  
  result = new PriceVector;
  if (retcode == TA_SUCCESS)
    for (qint32 counter = outNbElement - 1; counter >=0; counter --)
      result->append ((qreal) output[counter]);
  
  free ((void *) high);
  free ((void *) low);
  free ((void *) output);                     
  return result;    	
}

// relative strength index
DataSet 
RSI (DataSet dset, int period)
{
  PriceVector *result = NULL;
  float *input;
  double *output = NULL;
  TA_RetCode retcode;
  int outBegIdx, outNbElement;
  qint32 setsize;

  if (period < 2)
    period = 2;
  
  setsize = dset->size ();
  input = (float *) calloc (setsize + 1, sizeof (float));
  if (input == NULL)
    return NULL;
  
  output = (double *) calloc (setsize + 1, sizeof (double));
  if (output == NULL)
  {
	free ((void *) input);  
    return NULL;  
  }  
  
  for (qint32 counter = setsize; counter > 0; counter --)
    input[setsize - counter] = dset->at (counter - 1);
  
  retcode = TA_S_RSI (0, setsize - 1, input, period, 
                    &outBegIdx, &outNbElement, output);
  result = new PriceVector;
  
  if (retcode == TA_SUCCESS)
    for (qint32 counter = outNbElement - 1; counter >=0; counter --)
      result->append ((qreal) output[counter]);
  
  free ((void *) input);
  free ((void *) output);                     
  return result;    	
}

// moving average convergence/divergence 
DataSet 
MACD (DataSet dset, int period)
{
  PriceVector *result = NULL;
  float *input;
  double *signal = NULL, *hist = NULL, *macd = NULL;
  TA_RetCode retcode;
  int outBegIdx, outNbElement;
  qint32 setsize;

  if (period < 2)
    period = 2;
  
  setsize = dset->size ();
  input = (float *) calloc (setsize + 1, sizeof (float));
  if (input == NULL)
    return NULL;
  
  signal = (double *) calloc (setsize + 1, sizeof (double));
  if (signal == NULL)
  {
	free ((void *) input);  
    return NULL;  
  }  
  
  hist = (double *) calloc (setsize + 1, sizeof (double));
  if (hist == NULL)
  {
	free ((void *) input);  
	free ((void *) signal);  
    return NULL;  
  }
  
  macd = (double *) calloc (setsize + 1, sizeof (double));
  if (macd == NULL)
  {
	free ((void *) input);  
	free ((void *) signal);  
	free ((void *) hist);  
    return NULL;  
  }
  
  for (qint32 counter = setsize; counter > 0; counter --)
    input[setsize - counter] = dset->at (counter - 1);
  
  retcode = TA_S_MACDFIX (0, setsize - 1, input, period, 
                    &outBegIdx, &outNbElement, macd, signal, hist);
  result = new PriceVector;
  if (retcode == TA_SUCCESS)
  {
    for (qint32 counter = outNbElement - 1; counter >=0; counter --)
      if (qAbs ((qreal) macd[counter]) > 100000)
        result->append (0);
      else  
        result->append ((qreal) macd[counter]);
  }    
  
  free ((void *) input);
  free ((void *) signal);    
  free ((void *) hist);    
  free ((void *) macd);                      
  return result;    	
}

// moving average convergence/divergence signal
DataSet 
MACDSIGNAL (DataSet dset, int period)
{
  PriceVector *result = NULL;
  float *input;
  double *signal = NULL, *hist = NULL, *macd = NULL;
  TA_RetCode retcode;
  int outBegIdx, outNbElement;
  qint32 setsize;

  if (period < 2)
    period = 2;
  
  setsize = dset->size ();
  input = (float *) calloc (setsize + 1, sizeof (float));
  if (input == NULL)
    return NULL;
  
  signal = (double *) calloc (setsize + 1, sizeof (double));
  if (signal == NULL)
  {
	free ((void *) input);  
    return NULL;  
  }  
  
  hist = (double *) calloc (setsize + 1, sizeof (double));
  if (hist == NULL)
  {
	free ((void *) input);  
	free ((void *) signal);  
    return NULL;  
  }
  
  macd = (double *) calloc (setsize + 1, sizeof (double));
  if (macd == NULL)
  {
	free ((void *) input);  
	free ((void *) signal);  
	free ((void *) hist);  
    return NULL;  
  }
  
  for (qint32 counter = setsize; counter > 0; counter --)
    input[setsize - counter] = dset->at (counter - 1);
  
  retcode = TA_S_MACDFIX (0, setsize - 1, input, period, 
                    &outBegIdx, &outNbElement, macd, signal, hist);
  result = new PriceVector;
  if (retcode == TA_SUCCESS)
    for (qint32 counter = outNbElement - 1; counter >=0; counter --)
      result->append ((qreal) signal[counter]);
  
  free ((void *) input);
  free ((void *) signal);    
  free ((void *) hist);    
  free ((void *) macd);                      
  return result;    	
}

// moving average convergence/divergence histogram
DataSet 
MACDHIST (DataSet dset, int period)
{
  PriceVector *result = NULL;
  float *input;
  double *signal = NULL, *hist = NULL, *macd = NULL;
  TA_RetCode retcode;
  int outBegIdx, outNbElement;
  qint32 setsize;

  if (period < 2)
    period = 2;
  
  setsize = dset->size ();
  input = (float *) calloc (setsize + 1, sizeof (float));
  if (input == NULL)
    return NULL;
  
  signal = (double *) calloc (setsize + 1, sizeof (double));
  if (signal == NULL)
  {
	free ((void *) input);  
    return NULL;  
  }  
  
  hist = (double *) calloc (setsize + 1, sizeof (double));
  if (hist == NULL)
  {
	free ((void *) input);  
	free ((void *) signal);  
    return NULL;  
  }
  
  macd = (double *) calloc (setsize + 1, sizeof (double));
  if (macd == NULL)
  {
	free ((void *) input);  
	free ((void *) signal);  
	free ((void *) hist);  
    return NULL;  
  }
  
  for (qint32 counter = setsize; counter > 0; counter --)
    input[setsize - counter] = dset->at (counter - 1);
  
  retcode = TA_S_MACDFIX (0, setsize - 1, input, period, 
                    &outBegIdx, &outNbElement, macd, signal, hist);
  result = new PriceVector;
  if (retcode == TA_SUCCESS)
    for (qint32 counter = outNbElement - 1; counter >=0; counter --)
      result->append ((qreal) hist[counter]);
  
  free ((void *) input);
  free ((void *) signal);    
  free ((void *) hist);    
  free ((void *) macd);                      
  return result;    	
}

// bollinger bands upper
DataSet 
BBANDSUPPER (DataSet dset, int period)
{
  PriceVector *result = NULL;
  float *input;
  double *upper = NULL, *middle = NULL, *lower = NULL;
  TA_RetCode retcode;
  int outBegIdx, outNbElement;
  qint32 setsize;

  if (period < 2)
    period = 2;
  
  setsize = dset->size ();
  input = (float *) calloc (setsize + 1, sizeof (float));
  if (input == NULL)
    return NULL;
  
  upper = (double *) calloc (setsize + 1, sizeof (double));
  if (upper == NULL)
  {
	free ((void *) input);  
    return NULL;  
  }  
  
  middle = (double *) calloc (setsize + 1, sizeof (double));
  if (middle == NULL)
  {
	free ((void *) input);  
	free ((void *) middle);  
    return NULL;  
  }
  
  lower = (double *) calloc (setsize + 1, sizeof (double));
  if (lower == NULL)
  {
	free ((void *) input);  
	free ((void *) upper);  
	free ((void *) middle);  
    return NULL;  
  }
  
  for (qint32 counter = setsize; counter > 0; counter --)
    input[setsize - counter] = dset->at (counter - 1);
  
  retcode = TA_S_BBANDS (0, setsize - 1, input, period, 2.0, 2.0, 
            TA_MAType_SMA, &outBegIdx, &outNbElement, upper, middle, lower);
  result = new PriceVector;
  if (retcode == TA_SUCCESS)
    for (qint32 counter = outNbElement - 1; counter >=0; counter --)
      result->append ((qreal) upper[counter]);
  
  free ((void *) input);
  free ((void *) upper);    
  free ((void *) middle);    
  free ((void *) lower);                      
  return result;    	
}

// bollinger bands middle
DataSet 
BBANDSMIDDLE (DataSet dset, int period)
{
  PriceVector *result = NULL;
  float *input;
  double *upper = NULL, *middle = NULL, *lower = NULL;
  TA_RetCode retcode;
  int outBegIdx, outNbElement;
  qint32 setsize;

  if (period < 2)
    period = 2;
  
  setsize = dset->size ();
  input = (float *) calloc (setsize + 1, sizeof (float));
  if (input == NULL)
    return NULL;
  
  upper = (double *) calloc (setsize + 1, sizeof (double));
  if (upper == NULL)
  {
	free ((void *) input);  
    return NULL;  
  }  
  
  middle = (double *) calloc (setsize + 1, sizeof (double));
  if (middle == NULL)
  {
	free ((void *) input);  
	free ((void *) middle);  
    return NULL;  
  }
  
  lower = (double *) calloc (setsize + 1, sizeof (double));
  if (lower == NULL)
  {
	free ((void *) input);  
	free ((void *) upper);  
	free ((void *) middle);  
    return NULL;  
  }
  
  for (qint32 counter = setsize; counter > 0; counter --)
    input[setsize - counter] = dset->at (counter - 1);
  
  retcode = TA_S_BBANDS (0, setsize - 1, input, period, 2.0, 2.0, 
            TA_MAType_SMA, &outBegIdx, &outNbElement, upper, middle, lower);
  result = new PriceVector;
  if (retcode == TA_SUCCESS)
    for (qint32 counter = outNbElement - 1; counter >=0; counter --)
      result->append ((qreal) middle[counter]);
  
  free ((void *) input);
  free ((void *) upper);    
  free ((void *) middle);    
  free ((void *) lower);                      
  return result;    	
}

// bollinger bands lower
DataSet 
BBANDSLOWER (DataSet dset, int period)
{
  PriceVector *result = NULL;
  float *input;
  double *upper = NULL, *middle = NULL, *lower = NULL;
  TA_RetCode retcode;
  int outBegIdx, outNbElement;
  qint32 setsize;

  if (period < 2)
    period = 2;
  
  setsize = dset->size ();
  input = (float *) calloc (setsize + 1, sizeof (float));
  if (input == NULL)
    return NULL;
  
  upper = (double *) calloc (setsize + 1, sizeof (double));
  if (upper == NULL)
  {
	free ((void *) input);  
    return NULL;  
  }  
  
  middle = (double *) calloc (setsize + 1, sizeof (double));
  if (middle == NULL)
  {
	free ((void *) input);  
	free ((void *) middle);  
    return NULL;  
  }
  
  lower = (double *) calloc (setsize + 1, sizeof (double));
  if (lower == NULL)
  {
	free ((void *) input);  
	free ((void *) upper);  
	free ((void *) middle);  
    return NULL;  
  }
  
  for (qint32 counter = setsize; counter > 0; counter --)
    input[setsize - counter] = dset->at (counter - 1);
  
  retcode = TA_S_BBANDS (0, setsize - 1, input, period, 2.0, 2.0, 
            TA_MAType_SMA, &outBegIdx, &outNbElement, upper, middle, lower);
  result = new PriceVector;
  if (retcode == TA_SUCCESS)
    for (qint32 counter = outNbElement - 1; counter >=0; counter --)
      result->append ((qreal) lower[counter]);
  
  free ((void *) input);
  free ((void *) upper);    
  free ((void *) middle);    
  free ((void *) lower);                      
  return result;    	
}
