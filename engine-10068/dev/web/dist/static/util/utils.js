/**
 * 拼接对象为请求字符串
 * @param {Object} obj - 待拼接的对象
 * @returns {string} - 拼接成的请求字符串
 */
export function encodeSearchParams(obj) {
  const params = []

  for (item in obj) { //variable 为属性名
    let value = obj[item]
    // 如果值为undefined我们将其置空
    if (typeof value === 'undefined') {
      value = ''
    }
    // 对于需要编码的文本（比如说中文）我们要进行编码
    // params.push([item, encodeURIComponent(value)].join('='))
    params.push([item, value].join('='))
  }

  return params.join('&')
}
/**
 * 随机生成mac地址
 * @param {Object} obj - 待拼接的对象
 * @returns {string} - 拼接成的请求字符串
 */
export function mac() {
  var s = [];
  var hexDigits = "0123456789ABCDEFG";
  for (var i = 0; i < 16; i++) {
    s[i] = hexDigits.substr(Math.floor(Math.random() * 0x10), 1);
  }
  s[14] = "4"; // bits 12-15 of the time_hi_and_version field to 0010
  s[19] = hexDigits.substr((s[19] & 0x3) | 0x8, 1); // bits 6-7 of the clock_seq_hi_and_reserved to 01
  s[2] = s[5] = s[8] = s[11] = s[14] = ":";

  var mac = s.join("");
  return mac;
}
