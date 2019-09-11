#include <math.h>
#include <algorithm>
#include "IKInterval.h"
#include "ILog.h"

#define ABS(x) ((x) > 0 ? (x) : -(x))
namespace {
    class KaucherInterval: public virtual IKInterval
    {
    public:
        virtual int getId() const;

        static IKInterval* createInterval(double a, double b);


        /*operators
          to modify caller of method*/
        virtual int add(IKInterval const* const right);
        virtual int subtract(IKInterval const* const right);
        virtual int multiply(IKInterval const* const right);
        virtual int multiply(double right);
        virtual int divide(IKInterval const* const right);

        /*comparators*/
        /*The result argument passed by pointer, to have an ability to detect
          incomparable intervals with nullptr value (under discussion)*/
        virtual int gt(IKInterval const* const right, bool& result, COMP_MODE mode = COMP_DEFAULT) const;
        virtual int lt(IKInterval const* const right, bool& result, COMP_MODE mode = COMP_DEFAULT) const;
        virtual int eq(IKInterval const* const right, bool& result, COMP_MODE mode = COMP_DEFAULT) const;

        virtual int contains(IKInterval const* const right, bool& result) const;
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

        virtual IKInterval* clone() const;

        /*dtor*/
        virtual ~KaucherInterval(){}

        virtual int dual();
        virtual int pro();
        virtual int opp();
        virtual int inv();

        virtual int left(double res) const;
        virtual int right(double res) const;

    protected:
        KaucherInterval(double a, double b);
        double m_left, m_right;

    private:
    };
}

int KaucherInterval::getId() const
{
    return INTERFACE_0;
}

IKInterval* IKInterval::createInterval(double a, double b, CTORenum mode)
{
    if(mode == MIDDLE && !isnan(a-b) && !isnan(a+b))
    {
        return KaucherInterval::createInterval(a-b,a+b);
    }
    else
    {
        ILog::report("IKInterval.createInterval: double overfulls");
        return 0;
    }
    return KaucherInterval::createInterval(a,b);
}

IKInterval* KaucherInterval::createInterval(double a, double b)
{
    return new KaucherInterval(a,b);
}

KaucherInterval::KaucherInterval(double a, double b):m_left(a),m_right(b){}

int KaucherInterval::add(IKInterval const* const right)
{
    if(!right)
    {
        ILog::report("IKInterval.add: right operand of addition is nullptr, expected IKInterval pointer");
        return ERR_WRONG_ARG;
    }

    double r_a, r_b;
    right->left(r_a);
    right->right(r_b);
    if(isnan(m_left += r_a) || isnan(m_right += r_b))
    {
        ILog::report("IKInterval.add: double overfulls");
        return ERR_OVERFULL;
    }
    return ERR_OK;
}

int KaucherInterval::subtract(IKInterval const* const right)
{
    if(!right)
    {
        ILog::report("IKInterval.subtract: right operand of subtraction is nullptr, expected IKInterval pointer");
        return ERR_WRONG_ARG;
    }

    double r_a, r_b;
    right->left(r_a);
    right->right(r_b);
    if(isnan(m_left -= r_a) || isnan(m_right -= r_b))
    {
        ILog::report("IKInterval.subtract: double overfulls");
        return ERR_OVERFULL;
    }
    return ERR_OK;
}

int KaucherInterval::multiply(IKInterval const* const right)
{
    if(!right)
    {
        ILog::report("IKInterval.multiply: right operand of multiplication is nullptr, expected IKInterval pointer");
        return ERR_WRONG_ARG;
    }
    double tmp;
    double this_left, this_right;
    double r_a, r_b;
    right->left(r_a);
    right->right(r_b);

    if(m_left >= 0 && m_right >= 0)
    {
        if(r_a >= 0 && r_b >= 0)
        {
            if(isnan(m_left * r_a) || isnan(m_right * r_b))
            {
                ILog::report("IKInterval.multiply: double overfulls");
                return ERR_OVERFULL;
            }
            m_left *= r_a;
            m_right *= r_b;
        }
        else if(r_a <= 0 && 0 <= r_b)
        {
            if(isnan(m_right * r_a) || isnan(m_right * r_b))
            {
                ILog::report("IKInterval.multiply: double overfulls");
                return ERR_OVERFULL;
            }
            m_left = m_right * r_a;
            m_right *= r_b;
        }
        else if(r_a <= 0 && r_b <= 0)
        {
            if(isnan(m_right * r_a) || isnan(m_left * r_b))
            {
                ILog::report("IKInterval.multiply: double overfulls");
                return ERR_OVERFULL;
            }
            tmp = m_left;
            m_left = m_right * r_a;
            m_right = tmp * r_b;
        }
        else if(0 <= r_a && r_b <=0)
        {
            if(isnan(m_left * r_a) || isnan(m_left * r_b))
            {
                ILog::report("IKInterval.multiply: double overfulls");
                return ERR_OVERFULL;
            }
            m_right = m_left * r_b;
            m_left *= r_a;
        }
    }
    else if(m_left <= 0 && m_right <= 0)
    {
        if(r_a >= 0 && r_b >= 0)
        {
            if(isnan(m_left * r_b) || isnan(m_right * r_b))
            {
                ILog::report("IKInterval.multiply: double overfulls");
                return ERR_OVERFULL;
            }
            m_left *= r_b;
            m_right *= r_b;
        }
        else if(r_a <= 0 && 0 <= r_b)
        {
            if(isnan(m_left * r_b) || isnan(m_right * r_a) || isnan(m_left * r_a) || isnan(m_right * r_b))
            {
                ILog::report("IKInterval.multiply: double overfulls");
                return ERR_OVERFULL;
            }
            this_left = m_left, this_right = m_right;
            m_left = std::min(this_left*r_b, this_right*r_a);
            m_right = std::max(this_left*r_a,this_right*r_b);
        }
        else if(r_a <= 0 && r_b <= 0)
        {
            if(isnan(m_right * r_a) || isnan(m_left * r_a))
            {
                ILog::report("IKInterval.multiply: double overfulls");
                return ERR_OVERFULL;
            }
            tmp = m_left;
            m_left = m_right * r_a;
            m_right = tmp * r_a;
        }
        else if(0 <= r_a && r_b <=0)
        {
            m_left = m_right = 0;
        }
    }
    else if(m_left <= 0 && m_right <= 0)
    {
        if(r_a >= 0 && r_b >= 0)
        {
            if(isnan(m_left * r_b) || isnan(m_right * r_a))
            {
                ILog::report("IKInterval.multiply: double overfulls");
                return ERR_OVERFULL;
            }
            m_left *= r_b;
            m_right *= r_a;
        }
        else if(r_a <= 0 && 0 <= r_b)
        {
            if(isnan(m_left * r_b) || isnan(m_left * r_a))
            {
                ILog::report("IKInterval.multiply: double overfulls");
                return ERR_OVERFULL;
            }
            m_right = m_left * r_a;
            m_left *= r_b;

        }
        else if(r_a <= 0 && r_b <= 0)
        {
            if(isnan(m_right * r_b) || isnan(m_left * r_a))
            {
                ILog::report("IKInterval.multiply: double overfulls");
                return ERR_OVERFULL;
            }
            tmp = m_left;
            m_left = m_right * r_b;
            m_right = tmp * r_a;
        }
        else if(0 <= r_a && r_b <=0)
        {
            if(isnan(m_right * r_b) || isnan(m_right * r_a))
            {
                ILog::report("IKInterval.multiply: double overfulls");
                return ERR_OVERFULL;
            }
            m_left = m_right * r_b;
            m_right *= r_a;
        }
    }
    else if(0 <= m_left && m_right <= 0)
    {
        if(r_a >= 0 && r_b >= 0)
        {
            if(isnan(m_left * r_a) || isnan(m_right * r_a))
            {
                ILog::report("IKInterval.multiply: double overfulls");
                return ERR_OVERFULL;
            }
            m_left *= r_a;
            m_right *= r_a;
        }
        else if(r_a <= 0 && 0 <= r_b)
        {
            m_left = m_right = 0;
        }
        else if(r_a <= 0 && r_b <= 0)
        {
            if(isnan(m_right * r_b) || isnan(m_left * r_b))
            {
                ILog::report("IKInterval.multiply: double overfulls");
                return ERR_OVERFULL;
            }
            tmp = m_left;
            m_left = m_right * r_b;
            m_right = tmp * r_b;
        }
        else if(0 <= r_a && r_b <=0)
        {
            if(isnan(m_left * r_a) || isnan(m_left * r_b) || isnan(m_right * r_a) || isnan(m_right * r_b))
            {
                ILog::report("IKInterval.multiply: double overfulls");
                return ERR_OVERFULL;
            }
            this_left = m_left, this_right = m_right;
            m_left = std::max(this_left*r_a, this_right*r_b);
            m_right = std::min(this_left*r_b, this_right*r_a);
        }
    }


    return ERR_OK;
}

int KaucherInterval::multiply(double right)
{
    if(isnan(m_left * right) || isnan(m_right * right))
    {
        ILog::report("IKInterval.multiply: double overfulls");
        return ERR_OVERFULL;
    }
    if(right >= 0)
    {
        m_left *= right;
        m_right *= right;
    }
    else
    {
        dual();
        m_left *= right;
        m_right *= right;
    }
    return ERR_OK;
}

int KaucherInterval::divide(IKInterval const* const right)
{
    if(!right)
    {
        ILog::report("IKInterval.divide: right operand of division is nullptr, expected IKInterval pointer");
        return ERR_WRONG_ARG;
    }

    int lastError;
    bool res;
    IKInterval* copy_right = right->clone();
    if(!copy_right)
    {
        ILog::report("IKInterval.divide: memory allocation error");
        return ERR_MEMORY_ALLOCATION;
    }
    copy_right->pro();
    if((lastError = copy_right->contains(0.,res)) != ERR_OK)
    {
        ILog::report("IKInterval.divide: can't check containing");
        delete copy_right;
        copy_right = 0;
        return ERR_ANY_OTHER;
    }
    if(res)
    {
        ILog::report("IKInterval.divide: interval contains zero");
        delete copy_right;
        copy_right = 0;
        return ERR_DIV_BY_ZERO;
    }
    delete copy_right;
    copy_right = 0;
    copy_right = right->clone();
    if(!copy_right)
    {
        ILog::report("IKInterval.divide: memory allocation error");
        return ERR_MEMORY_ALLOCATION;
    }

    if((lastError = copy_right->inv()) != ERR_OK)
    {
        delete copy_right;
        copy_right = 0;
        return lastError;
    }
    copy_right->dual();
    multiply(copy_right);
    delete copy_right;
    copy_right = 0;
    return ERR_OK;
}

IKInterval* IKInterval::add(IKInterval const* const left, IKInterval const* const right)
{
    if (!left || !right)
    {
        ILog::report("IKInterval.add: operand of addition is nullptr, expected IKInterval pointer");
        return 0;
    }

    IKInterval* res = left->clone();
    if(!res)
    {
        return 0;
    }

    if (res->add(right) == ERR_OK)
        return res;
    else
        return 0;
}

IKInterval* IKInterval::subtract(IKInterval const* const left, IKInterval const* const right)
{
    if (!left || !right)
    {
        ILog::report("IKInterval.subtract: operand of subtraction is nullptr, expected IKInterval pointer");
        return 0;
    }

    IKInterval* res = left->clone();
    if(!res)
    {
        return 0;
    }

    if (res->subtract(right) == ERR_OK)
        return res;
    else
        return 0;
}

IKInterval* IKInterval::multiply(IKInterval const* const left, IKInterval const* const right)
{
    if (!left || !right)
    {
        ILog::report("IKInterval.multiply: operand of multiplication is nullptr, expected IKInterval pointer");
        return 0;
    }

    IKInterval* res = left->clone();
    if(!res)
    {
        return 0;
    }

    if (res->multiply(right) == ERR_OK)
        return res;
    else
        return 0;
}

IKInterval* IKInterval::multiply(const IKInterval *const left, double right)
{
    if (!left)
    {
        ILog::report("IKInterval.multiply: operand of division is nullptr, expected IKInterval pointer");
        return 0;
    }

    IKInterval* interv = IKInterval::createInterval(right, right, BOUNDS);
    if(!interv)
    {
        ILog::report("IKINterval.multiply: memory allocation error");
        return 0;
    }

    IKInterval* res = left->clone();
    if(!res)
    {
        ILog::report("IKINterval.multiply: memory allocation error");
        return 0;
    }

    if (res->multiply(interv) == ERR_OK)
    {   delete interv;
        return res;
    }
    else
    {
        delete interv;
        return 0;
    }
}

IKInterval* IKInterval::divide(IKInterval const* const left, IKInterval const* const right)
{
    if (!left || !right)
    {
        ILog::report("IKInterval.divide: operand of division is nullptr, expected IKInterval pointer");
        return 0;
    }

    IKInterval* res = left->clone();
    if(!res)
    {
        return 0;
    }

    if (res->divide(right) == ERR_OK)
        return res;
    else
        return 0;
}

int KaucherInterval::gt(IKInterval const* const right, bool& result, COMP_MODE mode) const
{
    if(!right)
    {
        ILog::report("IKInterval.gt: right operand is nullptr, expected IKInterval pointer");
        return ERR_WRONG_ARG;
    }

    double res1, res2;

    switch(mode)
    {
    case COMP_DEFAULT:
        right->sup(res1);
        inf(res2);
        result = res1 < res2;
        return ERR_OK;

    case COMP_DISPERSION:
        return ERR_NOT_IMPLEMENTED;

    case COMP_MEAN_EXTR:
        return ERR_NOT_IMPLEMENTED;

    case COMP_CONCENTRATION:
        right->inf(res1);
        right->sup(res2);
        if(res1 == res2 || m_left == m_right)
        {
            ILog::report("IKInterval.gt: cannot compute concentration of zero width interval");
            return ERR_ANY_OTHER;
        }
        double con1, con2;
        if(ABS(m_left) <= ABS(m_right))
            con1 = m_left/m_right;
        else
            con1 = m_right/m_left;

        if(ABS(res1) <= ABS(res2))
            con2 = res1/res2;
        else
            con2 = res2/res1;

        result = con1 > con2;
        return ERR_OK;

    case COMP_QUART:
        return ERR_NOT_IMPLEMENTED;
    }
    return ERR_OK;
}

int KaucherInterval::lt(IKInterval const* const right, bool& result, COMP_MODE mode) const
{
    if(!right)
    {
        ILog::report("IKInterval.lt: right operand is nullptr, expected IKInterval pointer");
        return ERR_WRONG_ARG;
    }

    double res1, res2;

    switch(mode)
    {
    case COMP_DEFAULT:
        right->inf(res1);
        sup(res2);
        result = res2 < res1;
        return ERR_OK;

    case COMP_DISPERSION:
        return ERR_NOT_IMPLEMENTED;

    case COMP_MEAN_EXTR:
        return ERR_NOT_IMPLEMENTED;

    case COMP_CONCENTRATION:
        right->inf(res1);
        right->sup(res2);
        if(res1 == res2 || m_left == m_right)
        {
            ILog::report("IKInterval.lt: cannot compute concentration of zero width interval");
            return ERR_ANY_OTHER;
        }
        double con1, con2;
        if(ABS(m_left) <= ABS(m_right))
            con1 = m_left/m_right;
        else
            con1 = m_right/m_left;

        if(ABS(res1) <= ABS(res2))
            con2 = res1/res2;
        else
            con2 = res2/res1;

        result = con1 < con2;
        return ERR_OK;

    case COMP_QUART:
        return ERR_NOT_IMPLEMENTED;
    }
    return ERR_OK;
}

int KaucherInterval::eq(IKInterval const* const right, bool& result, COMP_MODE mode) const
{
    if(!right)
    {
        ILog::report("IKInterval.eq: right operand is nullptr, expected IKInterval pointer");
        return ERR_WRONG_ARG;
    }

    double r_a, r_b;

    switch(mode)
    {
    case COMP_DEFAULT:
        right->left(r_a);
        right->right(r_b);
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
            ILog::report("IKInterval.eq: cannot compute concentration of zero width interval");
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

int KaucherInterval::contains(IKInterval const* const right, bool& result) const
{
    if(!right)
    {
        ILog::report("IKInterval.contains: right operand is nullptr, expected IKInterval pointer");
        return ERR_WRONG_ARG;
    }

    double r_a, r_b;
    right->left(r_a);
    right->right(r_b);

    result = (m_left <= r_a) && (r_b <= m_right);
    return ERR_OK;
}

int KaucherInterval::contains(double right, bool& result) const
{
    IKInterval* interv = IKInterval::createInterval(right,right,IKInterval::BOUNDS);
    if(!interv)
    {
        ILog::report("IKInterval.contains: memory allocation error");
    }
    int lastError = contains(interv, result);
    delete interv;
    return lastError;

}

int KaucherInterval::mid(double& res) const
{
    res = m_left/2 + m_right/2;
    return ERR_OK;
}

int KaucherInterval::wid(double& res) const
{
    if(isnan(ABS(m_right - m_left)))
    {
        ILog::report("IKInterval.wid: double overfulls");
        return ERR_OVERFULL;
    }
    res = ABS(m_right - m_left);
    return ERR_OK;
}

int KaucherInterval::rad(double& res) const
{
    if(isnan(ABS(m_right/2 - m_left/2)))
    {
        ILog::report("IKInterval.rad: double overfulls");
        return ERR_OVERFULL;
    }
    res = ABS(m_right/2 - m_left/2);
    return ERR_OK;
}

int KaucherInterval::inf(double& res) const
{
    res = std::min(m_left, m_right);
    return ERR_OK;
}

int KaucherInterval::sup(double& res) const
{
    res = std::max(m_left, m_right);
    return ERR_OK;
}

int KaucherInterval::magnitude(double& res) const
{
    res = std::max(ABS(m_left), ABS(m_right));
    return ERR_OK;
}
int KaucherInterval::mignitude(double& res) const
{
    bool b;
    contains(0.,b);
    if(b)
        res = 0;
    else
        std::min(ABS(m_left), ABS(m_right));
    return ERR_OK;
}

int KaucherInterval::dev(double& res) const
{
    if(ABS(m_left) >= ABS(m_right))
        res = m_left;
    else
        res = m_right;
    return ERR_OK;
}

int KaucherInterval::measureOfConcentration(double& res) const
{
    bool b;
    contains(0.,b);
    if(b)
    {
        ILog::report("IKInterval.measureOfConcentration: cannot compute concentration of zero width interval");
        return ERR_ANY_OTHER;
    }

    if(ABS(m_left) <= ABS(m_right))
        res = m_left/m_right;
    else
        res = m_right/m_left;
}

IKInterval* KaucherInterval::clone() const
{
    double r_a, r_b;
    left(r_a);
    right(r_b);
    return IKInterval::createInterval(r_a, r_b, BOUNDS);
}

int KaucherInterval::dual()
{
    m_left += m_right;
    m_right = m_left - m_right;
    m_left -= m_right;
    return ERR_OK;
}

int KaucherInterval::pro()
{
    if(m_left > m_right)
        return dual();
    return ERR_OK;
}

int KaucherInterval::opp()
{
    m_left = -m_left;
    m_right = -m_right;
    return ERR_OK;
}

int KaucherInterval::inv()
{
    bool res;
    IKInterval* zero = IKInterval::createInterval(0,0,BOUNDS);
    if(!zero)
    {
        ILog::report("IKInterval.inv: memory allocation error");
        return ERR_MEMORY_ALLOCATION;
    }
    int lastError;
    if((lastError = contains(zero, res)) != ERR_OK)
    {
        delete zero;
        zero = 0;
        return lastError;
    }
    if(!res)
    {
        if((lastError = zero->contains(this, res)) != ERR_OK)
        {
            delete zero;
            zero = 0;
            return lastError;
        }
        if(!res)
        {
            m_left = 1./m_left;
            m_right = 1./m_right;
            delete zero;
            zero = 0;
        }
        else
        {
            ILog::report("IKInterval.inv: can't inv interval contained in zero");
            delete zero;
            return ERR_DIV_BY_ZERO;
        }
    }
    else
    {
        ILog::report("IKInterval.inv: can't inv zero containing interval");
        delete zero;
        zero = 0;
        return ERR_DIV_BY_ZERO;
    }
    delete zero;
    zero = 0;
    return ERR_OK;
}

int KaucherInterval::left(double res) const
{
    res = m_left;
    return ERR_OK;
}

int KaucherInterval::right(double res) const
{
    res = m_right;
    return ERR_OK;
}

IKInterval* IKInterval::dual(IKInterval const* const other)
{
    IKInterval* interv = other->clone();
    if(!interv)
        return 0;
    interv->dual();
    return interv;
}

IKInterval* IKInterval::pro(IKInterval const* const other)
{
    IKInterval* interv = other->clone();
    if(!interv)
        return 0;
    interv->pro();
    return interv;
}

IKInterval* IKInterval::opp(IKInterval const* const other)
{
    IKInterval* interv = other->clone();
    if(!interv)
        return 0;
    interv->opp();
    return interv;
}

IKInterval* IKInterval::inv(IKInterval const* const other)
{
    IKInterval* interv = other->clone();
    if(!interv)
        return 0;
    interv->inv();
    return interv;
}
