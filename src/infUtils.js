const binding = require('./../build/Release/binding.node');

function mergeSets(set1, set2) {
  return new Set([...set1, ...set2]);
}

module.exports.parse = binding.parse;
module.exports.verify = binding.verify;

function detectVersion(platform) {
  const platformPattern = /^nt((?:x86)|(?:ia64)|(?:amd64))?(?:\.([\d]+))?(?:\.([\d]+))?(?:\.(0x[\d]{7}))?(?:\.(0x[\d]{8}))?$/;
  const versionsNumbers = [51, 52, 60, 61, 62, 63, 100];
  const versionsNames = ['xp', 'xp', 'vista', '7', '8', '8.1', '10'];

  const verInfo = platformPattern.exec(platform);

  const retSet = new Set();

  if (!verInfo) {
    return retSet;
  }

  let arch = 'x86'; // default arch

  if (verInfo[1]) {
    arch = verInfo[1];
  }

  const major = +verInfo[2] || 0;
  const minor = +verInfo[3] || 0;
  const fullVersion = +`${major}${minor}`;

  if (fullVersion === 0) { // supported all
    for (let i = 0; i < versionsNames.length; i++) {
      retSet.add(`${versionsNames[i]} ${arch}`);
    }
  } else {
    for (let i = 0; i < versionsNumbers.length; i++) {
      if (fullVersion === versionsNumbers[i]) {
        retSet.add(`${versionsNames[i]} ${arch}`);
        break;
      }
    }
  }

  return retSet;
}

module.exports.detectVersion = detectVersion;

function devicesInSection(section) {
  const retSet = new Set();

  for (let i = 0; i < section.length; i++) {
    const item = section[i].payload;
    const [, deviceId] = item.split(',');

    if (deviceId) {
      retSet.add(deviceId);
    }
  }

  return retSet;
}

module.exports.devicesInSection = devicesInSection;

function info(infPath) {
  const parsed = binding.parse(infPath);

  const platforms = new Set();
  let detected = new Set();
  let devices = new Set();

  const manufacturer = parsed.manufacturer;

  if (Array.isArray(manufacturer)) {
    for (let i = 0; i < manufacturer.length; i++) {
      const list = manufacturer[i].payload.split(',');
      const sectionPrefix = list[0].toLowerCase();

      if (sectionPrefix.length !== 0) {
        if (parsed[sectionPrefix]) {
          devices = mergeSets(devices, devicesInSection(parsed[sectionPrefix]));
        }

        for (let j = 1; j < list.length; j++) {
          const platform = list[j].toLowerCase();

          if (platform.length !== 0) {
            const section = `${sectionPrefix}.${platform}`;

            if (parsed[section]) { // check section exists
              platforms.add(platform);
              devices = mergeSets(devices, devicesInSection(parsed[section]));
              detected = mergeSets(detected, detectVersion(platform));
            }
          }
        }
      }
    }

    if (platforms.size === 0) { // if platform not specified, all x86 supported
      detected = detectVersion('nt');
    }
  }

  return {
    platforms: Array.from(platforms),
    detected: Array.from(detected),
    devices: Array.from(devices)
  };
}

module.exports.info = info;
