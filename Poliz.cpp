namespace Pawka {

Poliz::Poliz(map<string, Ident*>* a, map<string, int>* b,  
			map<string, string>* c, map<string, double>* d)
		: vars(a)
		, integers(b);
		, strings(c);
		, reals(d);
{	}

Poliz::~Poliz() {
    for (auto i : prog)
        delete i.second;
    for (auto i : st)
        delete i.second;
}

}//namespace
