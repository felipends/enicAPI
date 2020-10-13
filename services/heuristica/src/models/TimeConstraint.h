#ifndef TIME_CONSTRAINT_H
#define TIME_CONSTRAINT_H

class TimeConstraint {
    private:
        int day;
        int slot;
    public:
        TimeConstraint(int day, int slot);
        int getDay();
        int getSlot();
        bool compare(int day, int slot);
};

#endif