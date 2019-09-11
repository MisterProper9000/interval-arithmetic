#ifndef IKINTERVAL_H
#define IKINTERVAL_H

#include "IKInterval.h"

/*http://www.nsc.ru/interval/Library/InteBooks/SharyBook.pdf*/
class IKInterval
{
public:
    /*interface version enum*/
    enum InterfaceTypes{
        INTERFACE_0,
        DIMENSION_INTERFACE_IMPL,
    };

    /*ctor enum
      interval can be constructed by
      left and right bounds or center and radius.
      Since there are the same signatures, the enum
      will be needed*/
    enum CTORenum{
        MIDDLE,
        BOUNDS,
    };

    /*comparison enum
      default mode -- comparison of sup and inf of intervals
      Dispersion mode -- comparison of intervals length.
      MEAN_EXTR -- comparison of mean values of intervals bounds
      COMP_QUART -- comparison of interquartile ranges (in case of uniform
      distribution this is the same as MEAN_EXTR, and for now all intervals
      are uniformly distributed)
      CONCENTRATION -- comparison of a/b value of two [a, b] intervals */
    enum COMP_MODE {
        COMP_DEFAULT,
        COMP_DISPERSION,
        COMP_MEAN_EXTR,
        COMP_CONCENTRATION,
        COMP_QUART,
    };

    virtual int getId() const = 0;

    /*factories*/
    static IKInterval* createInterval(double a, double b, CTORenum mode);

    /*static operators
      to produce a new interval as a result*/
    static IKInterval* add(IKInterval const* const left, IKInterval const* const right);
    static IKInterval* subtract(IKInterval const* const left, IKInterval const* const right);
    static IKInterval* multiply(IKInterval const* const left, IKInterval const* const right);
    static IKInterval* multiply(IKInterval const* const left, double right);
    static IKInterval* divide(IKInterval const* const left, IKInterval const* const right);

    /*operators
      to modify caller of method*/
    virtual int add(IKInterval const* const right) = 0;
    virtual int subtract(IKInterval const* const right) = 0;
    virtual int multiply(IKInterval const* const right) = 0;
    virtual int multiply(double right) = 0;
    virtual int divide(IKInterval const* const right) = 0;

    /*comparators*/
    /*The result argument passed by pointer, to have an ability to detect
      incomparable intervals with nullptr value (under discussion)*/
    virtual int gt(IKInterval const* const right, bool& result, COMP_MODE mode = COMP_DEFAULT) const = 0;
    virtual int lt(IKInterval const* const right, bool& result, COMP_MODE mode = COMP_DEFAULT) const = 0;
    virtual int eq(IKInterval const* const right, bool& result, COMP_MODE mode = COMP_DEFAULT) const = 0;

    virtual int contains(IKInterval const* const right, bool& result) const = 0;
    virtual int contains(double right, bool& result) const = 0;

    virtual int mid(double& res) const = 0;
    virtual int wid(double& res) const = 0;
    virtual int rad(double& res) const = 0;
    virtual int inf(double& res) const = 0;
    virtual int sup(double& res) const = 0;
    virtual int magnitude(double& res) const = 0;
    virtual int mignitude(double& res) const = 0;
    virtual int dev(double& res) const = 0;
    virtual int measureOfConcentration(double& res) const = 0;

    virtual IKInterval* clone() const = 0;

    virtual int dual() = 0;
    virtual int pro() = 0;
    virtual int opp() = 0;
    virtual int inv() = 0;

    virtual int left(double res) const = 0;
    virtual int right(double res) const = 0;

    static IKInterval* dual(IKInterval const* const other);
    static IKInterval* pro(IKInterval const* const other);
    static IKInterval* opp(IKInterval const* const other);
    static IKInterval* inv(IKInterval const* const other);

    ~IKInterval(){}

protected:
    IKInterval() = default;

private:
    /*non default copyable*/
    IKInterval(const IKInterval& other) = delete;
    void operator=(const IKInterval& other) = delete;
};

#endif // IKINTERVAL_H
