/*Hi.Bob提供的滤波函数*/
#define DATA_MAX 10
int Filter(int direct)
{
  unsigned int val;
  unsigned int sum = 0;
  unsigned int maxVal = 0;
  unsigned int minVal = 0;
  for (int i = 0; i < DATA_MAX; i++) {
    val = analogRead(direct);
    sum += val;
    /* Get the max */
    if (val > maxVal)
      maxVal = val;
    /* Get the minVal */
    if (val < minVal)
      minVal = val;
  }
  /* Deal the maxVal and the minVal */
  sum -= maxVal;
  sum -= minVal;
  return sum / (DATA_MAX - 2);
}

