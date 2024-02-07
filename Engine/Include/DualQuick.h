#pragma once

template <typename T>
class DualQuick
{
public:
	DualQuick()
	{

	}
	~DualQuick()
	{

	}

public:

	static void Swap(T& a, T& b)
	{
		T iTemp = a;
		a = b;
		b = iTemp;
	}

	static void Patition(T Array[], int Left, int Right)
	{
		int iPivot1;
		int iPivot2;

		if (Left < Right)
		{
			iPivot1 = Array[Left];
			iPivot2 = Array[Right];

			if (iPivot1 > iPivot2)
			{
				Swap(Array[Left], Array[Right]);

				iPivot1 = Array[Left];
				iPivot2 = Array[Right];
			}
			// 여기까지 iPivot1 < iPivot2 가 되도록 한다.

			int i = Left + 1;
			int it = Left + 1;
			int gt = Right - 1;

			while (i <= gt)
			{
				if (Array[i] < iPivot1)
				{
					Swap(Array[i], Array[it]);

					i += 1;
					it += 1;
				}

				else if (iPivot2 < Array[i])
				{
					Swap(Array[i], Array[gt]);

					gt -= 1;
				}

				else
				{
					i += 1;
				}
			}


			it -= 1;
			Swap(Array[Left], Array[it]);
			gt += 1;
			Swap(Array[Right], Array[gt]);

			Patition(Array, Left, it - 1);
			Patition(Array, it + 1, gt - 1);
			Patition(Array, gt + 1, Right);
		}

		else
			return;
	}
};