/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) 2008-2018, Alliance for Sustainable Energy, LLC. All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
*  following conditions are met:
*
*  (1) Redistributions of source code must retain the above copyright notice, this list of conditions and the following
*  disclaimer.
*
*  (2) Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
*  disclaimer in the documentation and/or other materials provided with the distribution.
*
*  (3) Neither the name of the copyright holder nor the names of any contributors may be used to endorse or promote products
*  derived from this software without specific prior written permission from the respective party.
*
*  (4) Other than as required in clauses (1) and (2), distributions in any form of modifications or other derivative works
*  may not use the "OpenStudio" trademark, "OS", "os", or any other confusingly similar designation without specific prior
*  written permission from Alliance for Sustainable Energy, LLC.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER(S) AND ANY CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
*  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S), ANY CONTRIBUTORS, THE UNITED STATES GOVERNMENT, OR THE UNITED
*  STATES DEPARTMENT OF ENERGY, NOR ANY OF THEIR EMPLOYEES, BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
*  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
*  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
*  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
*  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***********************************************************************************************************************/

// TODO: QT-Separation-Move
#include "QJson.hpp"

#include "Assert.hpp"
#include "PathHelpers.hpp"
#include "FilesystemHelpers.hpp"

#include <OpenStudio.hxx>

#include <QJsonDocument>
#include <QJsonObject>

namespace openstudio {

QVariant jsonMetadata() {
  QVariantMap metadata;
  metadata["openstudio_version"] = toQString(openStudioVersion());
  return metadata;
}

bool saveJSON(const QVariant& json, openstudio::path p, bool overwrite) {
  // Ensures file extension is .json. Warns if there is a mismatch.
  p = setFileExtension(p,"json",true);

  openstudio::filesystem::ofstream file(p, std::ios_base::binary);
  if (file.is_open()) {
    QJsonDocument doc = QJsonDocument::fromVariant(json);
    const auto json = doc.toJson();
    file.write(json.constData(), json.size());
    file.close();

    return true;
  }

  LOG_FREE(Error,"openstudio.Json","Could not open file " << toString(p) << " for writing.");
  return false;
}

std::string toJSON(const QVariant& json) {
  QJsonDocument doc = QJsonDocument::fromVariant(json);

  return toString(QString(doc.toJson()));
}

QVariant loadJSON(const openstudio::path& p) {
  openstudio::filesystem::ifstream file(p, std::ios_base::binary);
  if (file.is_open()) {
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(openstudio::filesystem::read_as_QByteArray(file), &err);
    file.close();
    if (err.error) {
      LOG_FREE_AND_THROW("openstudio.Json","Error parsing JSON: " + toString(err.errorString()));
    }
    return doc.toVariant();
  }

  LOG_FREE_AND_THROW("openstudio.Json","Could not open file " << toString(p) << " for reading.");
  return QVariant();
}

QVariant loadJSON(const std::string& json) {
  QJsonDocument doc = QJsonDocument::fromJson(toQString(json).toUtf8());
  if (doc.isNull()) {
    LOG_FREE_AND_THROW("openstudio.Json","Error parsing JSON");
  }
  return doc.toVariant();
}

VersionString extractOpenStudioVersion(const QVariant& variant) {
  QJsonObject topLevel = QJsonDocument::fromVariant(variant).object();
  if (topLevel.contains("metadata")) {
    topLevel = topLevel["metadata"].toObject();
  }

  OptionalVersionString version;
  if (topLevel.contains("openstudio_version")) {
    version = VersionString(topLevel["openstudio_version"].toString().toStdString());
  }
  else if (topLevel.contains("version")) {
    version = VersionString(topLevel["version"].toString().toStdString());
  }
  else {
    LOG_FREE_AND_THROW("openstudio.core.Json","No version identifier found in QJSON variant.");
  }
  OS_ASSERT(version);
  if (version.get() > VersionString(openStudioVersion())) {
    LOG_FREE(Warn,"openstudio.Json","Loading json file from version " << version
             << " with OpenStudio version " << VersionString(openStudioVersion())
             << ". OpenStudio json files are not designed to be forwards-compatible. "
             << "Unexpected behavior may result.")
  }

  return version.get();
}

} // openstudio
