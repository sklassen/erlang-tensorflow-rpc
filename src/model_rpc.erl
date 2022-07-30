-module(model_rpc).
-export([start/0, start/1, stop/0]).
-export([reload/0,score/1, terminate/0]).
-define(TIMEOUT,3000).

start([]) -> start().
start() ->
    register(
        ?MODULE,
        spawn(fun() ->
            process_flag(trap_exit, true),
            Port = open_port({spawn, "./priv/ccpflow -d /tmp/tf-ar-a/ 2> /tmp/ccpflow.log"}, [{packet, 2}]),
            loop(Port)
        end)
    ).

reload() -> rpc({reload}).
score(X) -> rpc({score, X}).
terminate() -> rpc({exit}).

stop() ->
    ?MODULE ! stop.

rpc(Msg) ->
    ?MODULE ! {call, self(), Msg},
    receive
        {?MODULE, Result} ->
            Result
    after ?TIMEOUT
        -> erlang:error(timeout),
           ?MODULE ! stop
    end.

loop(Port) ->
    receive
        {call, Caller, Msg} ->
            Port ! {self(), {command, encode(Msg)}},
            receive
                {Port, {data, Data}} ->
                    Caller ! {?MODULE, decode(Data)}
            end,
            loop(Port);
        stop ->
            Port ! {self(), close},
            receive
                {Port, closed} ->
                    exit(normal)
            end;
        {'EXIT', Port, Reason} ->
            exit({port_terminated, Reason})
    end.

encode({reload}) -> [0];
encode({score, Xs}) -> [1|[X+127||X<-Xs]];
encode({exit}) -> [2].

decode(Ans) -> [Y/100||Y<-Ans].
