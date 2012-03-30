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

// calling a constructor with parameters
var str = stub.new('StubClass.Constructor', 4, 'string', [1,2,3]).call('ToString')
console.log(str);

str = stub.new('StubClass.Constructor', oc.call('ReturnObject')).call('ToString')
console.log(str);

// calling static methods
var ss = stub.static('StubClass.Static')
console.log('Static1: ' + ss.call('GetInt'));

console.log('Static2: '+ ss.call('Instance').call('InstanceString'));