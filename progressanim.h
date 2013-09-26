#ifndef PROGRESSANIM_H
#define PROGRESSANIM_H

class progressanim
{
public:
    progressanim();
    ~progressanim();

    void tick();


private:
    int status;
    char visual[4] = { '|', '/', '-', '\\' };
};

#endif // PROGRESSANIM_H
