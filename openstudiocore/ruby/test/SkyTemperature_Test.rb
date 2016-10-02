########################################################################################################################
#  OpenStudio(R), Copyright (c) 2008-2016, Alliance for Sustainable Energy, LLC. All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
#  following conditions are met:
#
#  (1) Redistributions of source code must retain the above copyright notice, this list of conditions and the following
#  disclaimer.
#
#  (2) Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
#  following disclaimer in the documentation and/or other materials provided with the distribution.
#
#  (3) Neither the name of the copyright holder nor the names of any contributors may be used to endorse or promote
#  products derived from this software without specific prior written permission from the respective party.
#
#  (4) Other than as required in clauses (1) and (2), distributions in any form of modifications or other derivative
#  works may not use the "OpenStudio" trademark, "OS", "os", or any other confusingly similar designation without
#  specific prior written permission from Alliance for Sustainable Energy, LLC.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
#  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER, THE UNITED STATES GOVERNMENT, OR ANY CONTRIBUTORS BE LIABLE FOR
#  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
#  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
########################################################################################################################

require 'openstudio'

require 'minitest/autorun'

class SkyTemperature_Test < MiniTest::Unit::TestCase

  def test_construction_and_parentage
  
    model = OpenStudio::Model::Model.new
    
    # no parent when no Site and parent not explicitly set
    skyTemperature = OpenStudio::Model::SkyTemperature.new(model)
    opo = skyTemperature.parent
    assert_equal(true,opo.empty?)
    
    site = model.getSite
    opo = skyTemperature.parent
    assert_equal(false,opo.empty?)
    skyTempParent = opo.get
    assert(skyTempParent == site)
    
    # make skyTemperature apply to a RunPeriod
    runPeriod = model.getRunPeriod
    assert(skyTemperature.setParent(runPeriod))
    assert_equal(0,site.children.size)
    assert_equal(1,runPeriod.children.size)
    
    # make new SkyTemperature that will be immediately parented by site
    globalSkyTemperature = OpenStudio::Model::SkyTemperature.new(model)
    opo = globalSkyTemperature.parent
    assert_equal(false,opo.empty?)
    globalSkyTempParent = opo.get
    assert(globalSkyTempParent == site)
    assert_equal(1,site.children.size)
    assert_equal(1,runPeriod.children.size)
  
  end
    
end