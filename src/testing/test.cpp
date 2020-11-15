#include <bits/stdc++.h>
using namespace std ;

int main()
{
	vector<int> a ;
	a.reserve(10) ;

	a.insert(a.begin()+5, 70) ;
	for(int x :a)
		cout << x << '\n' ;
}
