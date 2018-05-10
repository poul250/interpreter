namespace Pawka {

Poliz::~Poliz() {
    for (auto i : prog)
        delete i.second;
    for (auto i : st)
        delete i.second;
}

}//namespace
