const test = require('brittle')
const threadStats = require('.')

test('basic', (t) => {
  t.comment(threadStats())
})
