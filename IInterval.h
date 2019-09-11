#ifndef IINTERVAL_H
#define IINTERVAL_H

/*Both arithmetic and comparators methods still under discussion*/
class IInterval{
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
    static IInterval* createInterval(double a, double b, CTORenum mode);

    /*static operators
      to produce a new interval as a result*/
    static IInterval* add(IInterval const* const left, IInterval const* const right);
    static IInterval* subtract(IInterval const* const left, IInterval const* const right);
    static IInterval* multiply(IInterval const* const left, IInterval const* const right);
    static IInterval* multiply(IInterval const* const left, double right);
    static IInterval* divide(IInterval const* const left, IInterval const* const right);

    /*operators
      to modify caller of method*/
    virtual int add(IInterval const* const right) = 0;
    virtual int subtract(IInterval const* const right) = 0;
    virtual int multiply(IInterval const* const right) = 0;
    virtual int multiply(double right) = 0;
    virtual int divide(IInterval const* const right) = 0;

    /*comparators*/
    /*The result argument passed by pointer, to have an ability to detect
      incomparable intervals with nullptr value (under discussion)*/
    virtual int gt(IInterval const* const right, bool& result, COMP_MODE mode = COMP_DEFAULT) const = 0;
    virtual int lt(IInterval const* const right, bool& result, COMP_MODE mode = COMP_DEFAULT) const = 0;
    virtual int eq(IInterval const* const right, bool& result, COMP_MODE mode = COMP_DEFAULT) const = 0;

    virtual int contains(IInterval const* const right, bool& result) const = 0;
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

    virtual IInterval* clone() const = 0;

    /*dtor*/
    virtual ~IInterval(){}

};

#endif // IINTERVAL_H
