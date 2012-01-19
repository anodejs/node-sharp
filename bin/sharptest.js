var sharp = require('./Sharp').Sharp();
var stub = sharp.load('StubClass.dll');
var oc = stub.new('StubClass.Output');

console.log('Equals: ' + oc.call('Equals', oc));

console.log('Array: ' + oc.call('ReturnArray'));

console.log('List: ' + oc.call('ReturnList'));


oc.async('ReturnList', function(e, d){
	console.log('  Async callback');

	console.log('  Error: '+e+' Data: '+ d);
});


var ic = stub.new('StubClass.Input');

console.log('Object: ' + ic.call('AcceptObject', oc.call('ReturnObject')));