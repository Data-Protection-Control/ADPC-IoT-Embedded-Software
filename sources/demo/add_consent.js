var hypercore = require('hypercore')
var feed = hypercore('./ledger', {valueEncoding: 'utf-8'})

feed.append(process.argv.slice(2))

