__kernel void multMatrices(const __global int* mA,
						   const __global int* mB,
						   __global int* mC,
						   const int n)
{
	int row = get_global_id(0);
	int col = get_global_id(1);

	int sum = 0;

	for (int k = 0; k < n; k++)
	{
		sum += mA[row*n+k] * mB[k*n+col];
	}
	 
	mC[row * n + col] = sum;
}