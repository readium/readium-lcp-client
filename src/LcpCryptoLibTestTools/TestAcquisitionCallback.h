//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
//  Licensed under Gnu Affero General Public License Version 3 (provided, notwithstanding this notice,
//  Readium Foundation reserves the right to license this material under a different separate license,
//  and if you have done so, the terms of that separate license control and the following references
//  to GPL do not apply).
//
//  This program is free software: you can redistribute it and/or modify it under the terms of the GNU
//  Affero General Public License as published by the Free Software Foundation, either version 3 of
//  the License, or (at your option) any later version. You should have received a copy of the GNU
//  Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef __TEST_ACQUISITION_CALLBACK_H__
#define __TEST_ACQUISITION_CALLBACK_H__

#include <iostream>
#include "Public/lcp.h"

class TestAcquisitionCallback : public lcp::IAcquisitionCallback
{
public:
    virtual void OnAcquisitionStarted(lcp::IAcquisition * acquisition)
    {
        std::cout << "Acquisition started!" << std::endl;
    }
    virtual void OnAcquisitionProgressed(lcp::IAcquisition * acquisition, float progress)
    {
        std::cout << "\rAcquisition progressed!" << progress;
    }
    virtual void OnAcquisitionCanceled(lcp::IAcquisition * acquisition)
    {
        std::cout << std::endl << "Acquisition canceled!" << std::endl;
    }
    virtual void OnAcquisitionEnded(lcp::IAcquisition * acquisition, lcp::Status result)
    {
        std::cout << std::endl << "Acquisition ended! Status: " << result.ResultCode << " Ext: " << result.Extension << std::endl;
    }
};

#endif //__TEST_ACQUISITION_CALLBACK_H__
