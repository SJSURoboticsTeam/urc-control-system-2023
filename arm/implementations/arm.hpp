class arm{
    public:
    void move(){
        j.move();
        wrist.move();
    }

    private:
    joints j;
    wrist w;
}