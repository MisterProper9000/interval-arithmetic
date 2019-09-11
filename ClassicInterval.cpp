/*IMPORTANT NOTE:
  As inteface has the opportunity of comparing two intervals with info about
  distribution, this realisation doesn't use it, since knowledge of distribution (even uniform)
  involving moments of random variable and this is pretty tricky to implement for now
  */

#include <math.h>
#include <algorithm>
#include "IInterval.h"
#include "ILog.h"

#define ABS(x) ((x) > 0 ? (x) : -(x))

namespace {
    class ClassicInterval: public virtual IInterval
    {
    public:
        virtual int getId() const;

        static IInterval* createInterval(double a, double b);
        /*operators
          to modify caller of method*/
        virtual int add(IInterval const* const right);
        virtual int subtract(IInterval const* const right);
        virtual int multiply(IInterval const* const right);
        virtual int divide(IInterval const* const right);

        /*comparators*/
        /*The result argument passed by pointer, to have an ability to detect
          incomparable intervals with nullptr value (under discussion)*/
        virtual int gt(IInterval const* const right, bool& result, COMP_MODE mode = COMP_DEFAULT) const;
        virtual int lt(IInterval const* const right, bool& result, COMP_MODE mode = COMP_DEFAULT) const;
        virtual int eq(IInterval const* const right, bool& result, COMP_MODE mode = COMP_DEFAULT) const;

        virtual int contains(IInterval const* const right, bool& result) const;
        virtual int contains(double right, bool& result) const;

        virtual int mid(double& res) const;
        virtual int wid(double& res) const;
        virtual int rad(double& res) const;
        virtual int inf(double& res) const;
        virtual int sup(double& res) const;
        virtual int magnitude(double& res) const;
        virtual int mignitude(double& res) const;
        virtual int dev(double& res) const;
        virtual int measureOfConcentration(double& res) const;

        virtual IInterval* clone() const;

        /*dtor*/
        virtual ~ClassicInterval();

    protected:
        ClassicInterval(double a, double b);
        double m_left, m_right;
    };
}

int ClassicInterval::getId() const
{
    return INTERFACE_0;
}

IInterval* IInterval::createInterval(double a, double b, CTORenum mode)
{
    if(mode == MIDDLE && !isnan(a-b) && !isnan(a+b))
    {
        return ClassicInterval::createInterval(a-b,a+b);
    }
    else
    {
        ILog::report("IInterval.createInterval: double overfulls");
        return 0;
    }
    if(mode == BOUNDS && a<=b)
    {
        return ClassicInterval::createInterval(a,b);
    }
    else
    {
        ILog::report("IInterval.createInterval: left bound of interval can't be larger than right bound");
        return 0;
    }
}

IInterval* ClassicInterval::createInterval(double a, double b)
{
    return new ClassicInterval(a,b);
}

ClassicInterval::ClassicInterval(double a, double b):m_left(a),m_right(b){}

int ClassicInterval::add(IInterval const* const right)
{
    if(!right)
    {
        ILog::report("IInterval.add: right operand of addition is nullptr, expected IInterval pointer");
        return ERR_WRONG_ARG;
    }

    double r_a, r_b;
    right->inf(r_a);
    right->sup(r_b);
    if(isnan(m_left += r_a) || isnan(m_right += r_b))
    {
        ILog::report("IInterval.add: double overfulls");
        return ERR_OVERFULL;
    }
    return ERR_OK;
}

int ClassicInterval::subtract(IInterval const* const right)
{
    if(!right)
    {
        ILog::report("IInterval.subtract: right operand of subtraction is nullptr, expected IInterval pointer");
        return ERR_WRONG_ARG;
    }

    double r_a, r_b;
    right->inf(r_a);
    right->sup(r_b);
    if(isnan(m_left -= r_b) || isnan(m_right -= r_a))
    {
        ILog::report("IInterval.subtract: double overfulls");
        return ERR_OVERFULL;
    }
    return ERR_OK;
}

int ClassicInterval::multiply(IInterval const* const right)
{
    if(!right)
    {
        ILog::report("IInterval.multiply: right operand of multiplication is nullptr, expected IInterval pointer");
        return ERR_WRONG_ARG;
    }

    double r_a, r_b;
    right->inf(r_a);
    right->sup(r_b);
    if(isnan(m_left * r_a) || isnan(m_left * r_b) || isnan(m_right * r_a) || isnan(m_right * r_b))
    {
        ILog::report("IInterval.multiply: double overfulls");
        return ERR_OVERFULL;
    }
    m_left = std::min(std::min(m_left*r_a,m_left*r_b),std::min(m_right*r_a,m_right*r_b));
    m_right = std::max(std::max(m_left*r_a,m_left*r_b),std::max(m_right*r_a,m_right*r_b));

    return ERR_OK;
}

int ClassicInterval::divide(IInterval const* const right)
{
    if(!right)
    {
        ILog::report("IInterval.divide: right operand of division is nullptr, expected IInterval pointer");
        return ERR_WRONG_ARG;
    }

    bool res;
    right->contains(0.,res);
    if(res)
    {
        ILog::report("IInterval.divide: interval contains zero");
        return ERR_DIV_BY_ZERO;
    }

    double r_a, r_b, c;
    right->inf(r_a);
    c = r_a;
    r_a = 1/r_b;
    right->sup(r_b);
    r_b = 1/c;
    if(isnan(m_left * r_a) || isnan(m_left * r_b) || isnan(m_right * r_a) || isnan(m_right * r_b))
    {
        ILog::report("IInterval.divide: double overfulls");
        return ERR_OVERFULL;
    }
    m_left = std::min(std::min(m_left*r_a,m_left*r_b),std::min(m_right*r_a,m_right*r_b));
    m_right = std::max(std::max(m_left*r_a,m_left*r_b),std::max(m_right*r_a,m_right*r_b));

    return ERR_OK;
}

IInterval* IInterval::add(IInterval const* const left, IInterval const* const right)
{
    if (!left || !right)
    {
        ILog::report("IInterval.add: operand of addition is nullptr, expected IInterval pointer");
        return 0;
    }

    IInterval* res = left->clone();
    if(!res)
    {
        return 0;
    }

    if (res->add(right) == ERR_OK)
        return res;
    else
        return 0;
}

IInterval* IInterval::subtract(IInterval const* const left, IInterval const* const right)
{
    if (!left || !right)
    {
        ILog::report("IInterval.subtract: operand of subtraction is nullptr, expected IInterval pointer");
        return 0;
    }

    IInterval* res = left->clone();
    if(!res)
    {
        return 0;
    }

    if (res->subtract(right) == ERR_OK)
        return res;
    else
        return 0;
}

IInterval* IInterval::multiply(IInterval const* const left, IInterval const* const right)
{
    if (!left || !right)
    {
        ILog::report("IInterval.multiply: operand of multiplication is nullptr, expected IInterval pointer");
        return 0;
    }

    IInterval* res = left->clone();
    if(!res)
    {
        return 0;
    }

    if (res->multiply(right) == ERR_OK)
        return res;
    else
        return 0;
}

IInterval* IInterval::divide(IInterval const* const left, IInterval const* const right)
{
    if (!left || !right)
    {
        ILog::report("IInterval.divide: operand of division is nullptr, expected IInterval pointer");
        return 0;
    }

    IInterval* res = left->clone();
    if(!res)
    {
        return 0;
    }

    if (res->divide(right) == ERR_OK)
        return res;
    else
        return 0;
}

int ClassicInterval::gt(IInterval const* const right, bool& result, COMP_MODE mode) const
{
    if(!right)
    {
        ILog::report("IInterval.gt: right operand is nullptr, expected IInterval pointer");
        return ERR_WRONG_ARG;
    }

    double r_a, r_b;

    switch(mode)
    {
    case COMP_DEFAULT:
        right->sup(r_b);
        result = r_b < m_left;
        return ERR_OK;

    case COMP_DISPERSION:
        return ERR_NOT_IMPLEMENTED;

    case COMP_MEAN_EXTR:
        return ERR_NOT_IMPLEMENTED;

    case COMP_CONCENTRATION:
        right->inf(r_a);
        right->sup(r_b);
        if(r_a == r_b || m_left == m_right)
        {
            ILog::report("IInterval.gt: cannot compute concentration of zero width interval");
            return ERR_ANY_OTHER;
        }
        double con1, con2;
        if(ABS(m_left) <= ABS(m_right))
            con1 = m_left/m_right;
        else
            con1 = m_right/m_left;

        if(ABS(r_a) <= ABS(r_b))
            con2 = r_a/r_b;
        else
            con2 = r_b/r_a;

        result = con1 > con2;
        return ERR_OK;

    case COMP_QUART:
        return ERR_NOT_IMPLEMENTED;
    }
    return ERR_OK;
}

int ClassicInterval::lt(IInterval const* const right, bool& result, COMP_MODE mode) const
{
    if(!right)
    {
        ILog::report("IInterval.lt: right operand is nullptr, expected IInterval pointer");
        return ERR_WRONG_ARG;
    }

    double r_a, r_b;

    switch(mode)
    {
    case COMP_DEFAULT:
        right->inf(r_a);
        result = m_right < r_a;
        return ERR_OK;

    case COMP_DISPERSION:
        return ERR_NOT_IMPLEMENTED;

    case COMP_MEAN_EXTR:
        return ERR_NOT_IMPLEMENTED;

    case COMP_CONCENTRATION:
        right->inf(r_a);
        right->sup(r_b);
        if(r_a == r_b || m_left == m_right)
        {
            ILog::report("IInterval.gt: cannot compute concentration of zero width interval");
            return ERR_ANY_OTHER;
        }
        double con1, con2;
        if(ABS(m_left) <= ABS(m_right))
            con1 = m_left/m_right;
        else
            con1 = m_right/m_left;

        if(ABS(r_a) <= ABS(r_b))
            con2 = r_a/r_b;
        else
            con2 = r_b/r_a;

        result = con1 < con2;
        return ERR_OK;

    case COMP_QUART:
        return ERR_NOT_IMPLEMENTED;
    }
    return ERR_OK;
}

int ClassicInterval::eq(IInterval const* const right, bool& result, COMP_MODE mode) const
{
    if(!right)
    {
        ILog::report("IInterval.eq: right operand is nullptr, expected IInterval pointer");
        return ERR_WRONG_ARG;
    }

    double r_a, r_b;

    switch(mode)
    {
    case COMP_DEFAULT:
        right->inf(r_a);
        right->sup(r_b);
        result = (r_a == m_left) && (r_b == m_right);
        return ERR_OK;

    case COMP_DISPERSION:
        return ERR_NOT_IMPLEMENTED;

    case COMP_MEAN_EXTR:
        return ERR_NOT_IMPLEMENTED;

    case COMP_CONCENTRATION:
        right->inf(r_a);
        right->sup(r_b);
        if(r_a == r_b || m_left == m_right)
        {
            ILog::report("IInterval.gt: cannot compute concentration of zero width interval");
            return ERR_ANY_OTHER;
        }
        double con1, con2;
        if(ABS(m_left) <= ABS(m_right))
            con1 = m_left/m_right;
        else
            con1 = m_right/m_left;

        if(ABS(r_a) <= ABS(r_b))
            con2 = r_a/r_b;
        else
            con2 = r_b/r_a;

        result = con1 == con2;
        return ERR_OK;

    case COMP_QUART:
        return ERR_NOT_IMPLEMENTED;
    }
    return ERR_OK;
}

int ClassicInterval::contains(IInterval const* const right, bool& result) const
{
    if(!right)
    {
        ILog::report("IInterval.contains: right operand is nullptr, expected IInterval pointer");
        return ERR_WRONG_ARG;
    }

    double r_a, r_b;
    right->inf(r_a);
    right->sup(r_b);

    result = (m_left <= r_a) && (r_b <= m_right);
    return ERR_OK;
}

int ClassicInterval::contains(double right, bool& result) const
{
    result = (m_left <= right) && (right <= m_right);
    return ERR_OK;
}

int ClassicInterval::mid(double& res) const
{
    res = m_left/2 + m_right/2;
    return ERR_OK;
}

int ClassicInterval::wid(double& res) const
{
    if(isnan(m_right - m_left))
    {
        ILog::report("IInterval.subtract: double overfulls");
        return ERR_OVERFULL;
    }
    res = m_right - m_left;
    return ERR_OK;
}

int ClassicInterval::rad(double& res) const
{
    if(isnan(m_right/2 - m_left/2))
    {
        ILog::report("IInterval.subtract: double overfulls");
        return ERR_OVERFULL;
    }
    res = m_right/2 - m_left/2;
    return ERR_OK;
}

int ClassicInterval::inf(double& res) const
{
    res = m_left;
    return ERR_OK;
}

int ClassicInterval::sup(double& res) const
{
    res = m_right;
    return ERR_OK;
}

int ClassicInterval::magnitude(double& res) const
{
    res = std::max(ABS(m_left), ABS(m_right));
    return ERR_OK;
}
int ClassicInterval::mignitude(double& res) const
{
    bool b;
    contains(0.,b);
    if(b)
        res = 0;
    else
        std::min(ABS(m_left), ABS(m_right));
    return ERR_OK;
}

int ClassicInterval::dev(double& res) const
{
    if(ABS(m_left) >= ABS(m_right))
        res = m_left;
    else
        res = m_right;
    return ERR_OK;
}

int ClassicInterval::measureOfConcentration(double& res) const
{
    bool b;
    contains(0.,b);
    if(b)
    {
        ILog::report("IInterval.gt: cannot compute concentration of zero width interval");
        return ERR_ANY_OTHER;
    }

    if(ABS(m_left) <= ABS(m_right))
        res = m_left/m_right;
    else
        res = m_right/m_left;
}

IInterval* ClassicInterval::clone() const
{
    double r_a, r_b;
    inf(r_a);
    sup(r_b);
    return IInterval::createInterval(r_a, r_b, BOUNDS);
}

ClassicInterval::~ClassicInterval(){}
