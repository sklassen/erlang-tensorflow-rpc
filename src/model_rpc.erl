-module(model_rpc).
-export([start/0, start/1, stop/0]).
-export([reload/1,score/2, terminate/1]).
-define(TIMEOUT,3000).

start([]) -> start().
start() ->
    spawn(fun() ->
        process_flag(trap_exit, true),
        Port = open_port({spawn, "./priv/ccpflow -d /tmp/tf-ar-a/ 2> /tmp/ccpflow.log"}, [{packet, 2}]),
        loop(Port)
    end).

reload(Pid) -> rpc(Pid,{reload}).
score(Pid,X) -> rpc(Pid,{score, X}).
terminate(Pid) -> rpc(Pid,{exit}).

stop() ->
    ?MODULE ! stop.

rpc(Pid,Msg) ->
    Pid ! {call, self(), Msg},
    receive
        {Pid, Result} ->
            Result
    after ?TIMEOUT
        -> erlang:error(timeout)
    end.

loop(Port) ->
    receive
        {call, Caller, Msg} ->
            Port ! {self(), {command, encode(Msg)}},
            receive
                {Port, {data, Data}} ->
                    Caller ! {self(), decode(Data)}
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
