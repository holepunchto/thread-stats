const test = require('brittle')
const { isWindows } = require('which-runtime')
const threadStats = require('.')

test('basic', { skip: isWindows }, (t) => {
  t.comment(threadStats())
})
