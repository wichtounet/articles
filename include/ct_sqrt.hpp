#ifndef ARTICLES_GRAPHS
#define ARTICLES_GRAPHS

static constexpr std::size_t ct_mid(std::size_t a, std::size_t b){
    return (a+b) / 2;
}

static constexpr std::size_t ct_pow(std::size_t a){
    return a*a;
}

static constexpr std::size_t ct_sqrt(std::size_t res, std::size_t l, std::size_t r){
    return
        l == r ? r
        : ct_sqrt(res, ct_pow(ct_mid(r, l)) >= res ? l : ct_mid(r, l) + 1, ct_pow(ct_mid(r, l)) >= res ? ct_mid(r, l) : r);
}

static constexpr std::size_t ct_sqrt(std::size_t res){
    return ct_sqrt(res, 1, res);
}


#endif