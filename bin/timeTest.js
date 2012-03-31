//************
// Script to get expected timing for node-sharp methods
//

function time(method)
{
    var start = new Date().getTime();
    method();
    var end = new Date().getTime();
    
    return end - start;
}

function time_async(method, cb)
{
    var start = new Date().getTime();
    method(function(){
        var end = new Date().getTime();
        cb(end-start);
    });
}

function repeat(method, times)
{
    if (!times) times = 1;
    for(var i=0; i < times; i++)
    {
        method();
    }
}

var sharp;

var startup_time = time(function() {
    sharp = require('./Sharp').Sharp();
});

// first run 600ms, later runs 100ms
console.log('startup time: '+ startup_time);

var list_time = time(function() {
    sharp.list();
});

// first run 26ms, later runs 7ms
console.log('list time: '+ list_time);

var stub;
var load_time = time(function() {
    stub = sharp.load('StubClass.dll');
});

// first run 29ms, later runs 11ms
console.log('load time: '+load_time);

var o;
var new_time = time(function() {
    o = stub.new('StubClass.Output');
});

// first run 5ms, later runs 5ms
console.log('new time: '+new_time);

var rep = 1000;
var first_time =   time(function() { o.call('ReturnInt'); });
var int_time =   time(function() { repeat(function() { o.call('ReturnInt'); }, rep);});
var str_time =   time(function() { repeat(function() { o.call('ReturnString'); }, rep);});
var arr_time =   time(function() { repeat(function() { o.call('ReturnArray'); }, rep);});
var lst_time =   time(function() { repeat(function() { o.call('ReturnList'); }, rep);});
var obj_time =   time(function() { repeat(function() { o.call('ReturnObject'); }, rep);});

console.log('int time: '+ int_time);    // 50ms / 10000
console.log('string time: '+ str_time); // 60ms / 10000
console.log('array time: '+ arr_time);  // 100ms / 10000
console.log('list time: '+ lst_time);   // 160ms / 10000
console.log('object time: '+ obj_time); // 630ms / 10000

var ip = stub.new('StubClass.Input');
var obj = o.call('ReturnObject');

var first_in_time = time(function() { ip.call('AcceptInt', 1); });
var int_in_time =   time(function() { repeat(function() { ip.call('AcceptInt', 1); }, rep);});
var str_in_time =   time(function() { repeat(function() { ip.call('AcceptString', 'string'); }, rep);});
var arr_in_time =   time(function() { repeat(function() { ip.call('AcceptArray', [1,2,3]); }, rep);});
var lst_in_time =   time(function() { repeat(function() { ip.call('AcceptList', [1,2,3]); }, rep);});
var obj_in_time =   time(function() { repeat(function() { ip.call('AcceptObject', obj); }, rep);});

console.log('int input time: '+ int_in_time);    // 65ms / 10000
console.log('string input time: '+ str_in_time); // 80ms / 10000
console.log('array input time: '+ arr_in_time);  // 190ms / 10000
console.log('list input time: '+ lst_in_time);   // 330ms / 10000
console.log('object input time: '+ obj_in_time); // 630ms / 10000

// for comparison - all javascript (1000x faster)
rep = 1000000;
var first_js_time = time(function() { return 1; });
var int_js_time =   time(function() { repeat(function() { return 1; }, rep);});
var str_js_time =   time(function() { repeat(function() { return 'string'; }, rep);});
var arr_js_time =   time(function() { repeat(function() { return [1,2,3]; }, rep);});
var obj_js_time =   time(function() { repeat(function() { return {a : "a", b : 1}; }, rep);});

console.log('int javascript time: '+ int_js_time);    // 8ms / 1,000,000
console.log('string javascript time: '+ str_js_time); // 8ms /  1,000,000
console.log('array javascript time: '+ arr_js_time);  // 36ms / 1,000,000
console.log('object javascript time: '+ obj_js_time); // 54ms / 1,000,000

// Async!
var async_time = 0;
var async_active = 0;
var aStart = new Date().getTime();
repeat(function() {
    time_async(function(cb) { async_active++; o.async('ReturnObject', cb); }, 
               function(time) { async_time += time; 
                   if (--async_active == 0) { 
                     console.log('async time: ' +(new Date().getTime() - aStart) + " total: "+  async_time);
                   }
               }); 
}, 1000);
