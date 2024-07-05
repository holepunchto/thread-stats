const test = require('brittle')
const { isMac } = require('which-runtime')
const threadStats = require('.')

test('basic', { skip: !isMac }, (t) => {
  t.comment(threadStats())
})
