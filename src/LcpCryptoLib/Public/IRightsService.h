#ifndef __I_RIGHTS_SERVICE_H__
#define __I_RIGHTS_SERVICE_H__

#include <string>

namespace lcp
{
    class ILicense;

    class IRightsService
    {
    public:
        // Returns whether the user has the given right. Only works with
        // the rights identifiers defined in the specification.
        // Eg. for "copy", returns whether the User can still copy text
        // from the publication.
        // For "end", returns whether the License isn't expired.
        // If the right identifier can't be found in the License, should
        // return true.
        virtual bool HasRight(ILicense * license, const std::string & rightId) const = 0;

        // Consumes one or more right units. Only works with rights with
        // a set amount, e.g. copy and print. If no amount is given, assume 1 unit.
        // if units count is not enough return false
        virtual bool Consume(ILicense * license, const std::string & rightId) = 0;
        virtual bool Consume(ILicense * license, const std::string & rightId, int amount) = 0;

        // Stores the given JSON value in the secure storage for the 
        // given license and right identifier. It can be used by Clients 
        // to implement custom rights.
        virtual void SetValue(ILicense * license, const std::string & rightId, const std::string & value) = 0;

        // Retrieves the current JSON value in the storage provider 
        // associated with the given license and right identifier. It can
        // be used by Clients to implement custom rights.
        virtual std::string GetValue(ILicense * license, const std::string & rightId) const = 0;

        virtual ~IRightsService() {}
    };
}

#endif //__I_RIGHTS_SERVICE_H__
