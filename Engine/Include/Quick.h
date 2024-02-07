#pragma once

class QuickSort
{
public:

	static bool Funtion(int src, int dest)
	{
		return src < dest;
	}
	//src = Source
	//dest  = Destination

	static void Swap(int& num1, int& num2)
	{
		int temp = num1;
		num1 = num2;
		num2 = temp;
	}

	static void Patition(int sort[], int Left, int Right)
	{
		if (Left < Right)
		{
			int Mid = Pivot(sort, Left, Right, Funtion);
			Patition(sort, Left, Mid - 1);
			Patition(sort, Mid + 1, Right);
		}
	}

	static int Pivot(int sort[], int Left, int Right, bool(*Func)(int, int))
	{
		int L = Left;
		int R = Right;
		int pivot = sort[Right];

		while ( L < R )
		{
			while ( Func(sort[L], pivot) )
			{
				L++;
			}
			while ( Func(pivot, sort[R]) || sort[R] == pivot )
			{
				R--;
			}

			if ( L < R )
			{
				Swap( sort[L], sort[R] );
			}

		}

		Swap( sort[L], sort[Right] );

		return L;
	}
};