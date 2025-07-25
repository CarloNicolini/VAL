// Copyright 2019 - University of Strathclyde, King's College London and Schlumberger Ltd
// This source code is licensed under the BSD license found in the LICENSE file in the root directory of this source tree.

#ifndef __REPAIRADVICE
#define __REPAIRADVICE
#include "Action.h"
#include "State.h"
#include <memory>

using std::auto_ptr;

namespace VAL {

class UnsatCondition {
protected:
    mutable State state;

private:
    // Don't want these to be allowed.
    UnsatCondition(const UnsatCondition &us);
    UnsatCondition &operator=(const UnsatCondition &us);

public:
    const AdviceProposition *ap;
    UnsatCondition(const State &st, const AdviceProposition *a)
        : state(st), ap(a) {};
    virtual ~UnsatCondition() {
        delete ap;
    };
    virtual void display() const {};
    virtual void advice() const {};
    virtual const Action *getAct() const {
        return 0;
    };
    virtual double getTime() const {
        return 0;
    };
    virtual State &getState() const {
        return state;
    };
    virtual double howLong() const {
        return 0.0;
    };
    virtual string getDisplayString() const {
        return "!";
    };
    virtual string getAdviceString() const;
};

struct UnsatPrecondition : public UnsatCondition {
    double time;
    const Action *action;

    UnsatPrecondition(double t, const Action *a, const State *s)
        : UnsatCondition(*s, a->getPrecondition()->getAdviceProp(s)),
          time(t),
          action(a) {};
    ~UnsatPrecondition() {};
    void display() const;
    void advice() const;
    const Action *getAct() const {
        return action;
    };
    double getTime() const {
        return time;
    };
    string getDisplayString() const;
};
struct UnsatDurationCondition : public UnsatCondition {
    double time;
    const Action *action;
    double error;  // how far out was the duration?

    UnsatDurationCondition(double t, const Action *a, const State *s, double e)
        : UnsatCondition(*s, 0), time(t), action(a), error(e) {};

    ~UnsatDurationCondition() {};
    void display() const;
    string getDisplayString() const;
    void advice() const;
};

struct MutexViolation : public UnsatCondition {
    double time;
    const Action *action1;
    const Action *action2;

    // string reason; //reason for the mutex condition
    MutexViolation(double t, const Action *a1, const Action *a2, const State *s)
        : UnsatCondition(*s, 0), time(t), action1(a1), action2(a2) {};

    ~MutexViolation() {};
    void display() const;
    string getDisplayString() const;
    void advice() const;
};

struct UnsatGoal : public UnsatCondition {
    const Proposition *pre;

    UnsatGoal(const Proposition *p, const State *s)
        : UnsatCondition(*s, p->getAdviceProp(s)), pre(p) {};
    ~UnsatGoal() {
        pre->destroy();
    };
    void display() const;
    string getDisplayString() const;
    void advice() const;
};

struct UnsatInvariant : public UnsatCondition {
    double startTime;
    double endTime;
    Intervals satisfiedOn;
    const Action *action;  // invariant Action, precondition is unsatisfied
    // condition
    bool rootError;

    UnsatInvariant(double st, double e, const Intervals &ints, const Action *a,
                   const State *s, bool re)
        : UnsatCondition(*s, a->getPrecondition()->getAdviceProp(s)),
          startTime(st),
          endTime(e),
          satisfiedOn(ints),
          action(a),
          rootError(re) {};
    ~UnsatInvariant() {};
    void display() const;
    void advice() const;
    const Action *getAct() const {
        return action;
    };
    double getTime() const {
        return startTime;
    };
    double getEnd() const {
        return endTime;
    };
    bool isRootError() const {
        return rootError;
    };
    const Intervals &getInts() const {
        return satisfiedOn;
    };
    double howLong() const {
        return endTime - startTime;
    };
    string getDisplayString() const;
};

struct UnsatConditionFactory {
    virtual ~UnsatConditionFactory() {};
    virtual UnsatPrecondition *buildUnsatPrecondition(double t, const Action *a,
            const State *s) {
        return new UnsatPrecondition(t, a, s);
    };
    virtual UnsatDurationCondition *buildUnsatDurationCondition(double t,
            const Action *a,
            const State *s,
            double e) {
        return new UnsatDurationCondition(t, a, s, e);
    };
    virtual MutexViolation *buildMutexViolation(double t, const Action *a1,
            const Action *a2,
            const State *s) {
        return new MutexViolation(t, a1, a2, s);
    };
    virtual UnsatGoal *buildUnsatGoal(const Proposition *p, const State *s) {
        return new UnsatGoal(p, s);
    };
    virtual UnsatInvariant *buildUnsatInvariant(double st, double e,
            const Intervals &ints,
            const Action *a, const State *s,
            bool re) {
        return new UnsatInvariant(st, e, ints, a, s, re);
    };
};

class ErrorLog {
private:
    static std::unique_ptr< UnsatConditionFactory > fac;

    vector< const UnsatCondition * > conditions;

public:
    template < typename Fac >
    static void replace() {
        fac = std::make_unique<Fac>();
    };
    template < typename Fac >
    static void replace(std::unique_ptr<Fac> f) {
        fac = std::move(f);
    };
    ErrorLog() {};
    ~ErrorLog();
    void addPrecondition(double t, const Action *a, const State *s);
    void addUnsatDurationCondition(double t, const Action *a, const State *s,
                                   double e);
    void addMutexViolation(double t, const Action *a1, const Action *a2,
                           const State *s);
    void addUnsatInvariant(double st, double e, Intervals ints, const Action *a,
                           const State *s, bool rootError = false);
    void addGoal(const Proposition *p, const State *s);
    vector< const UnsatCondition * > &getConditions() {
        return conditions;
    };
    // vector<const UnsatCondition *> getConditions() const {return
    // conditions;};
    void displayReport() const;
};

};  // namespace VAL

#endif
