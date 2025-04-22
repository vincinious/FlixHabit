import React, { createContext, useContext, useState } from "react";

const TabsCtx = createContext();

export function Tabs({ value, onValueChange, children, className = "" }) {
  const [internal, setInternal] = useState(value);
  const current = value ?? internal;
  const set = onValueChange ?? setInternal;

  return (
    <TabsCtx.Provider value={{ current, set }}>
      <div className={className}>{children}</div>
    </TabsCtx.Provider>
  );
}

export function TabsList({ children, className = "" }) {
  return (
    <div
      className={`flex gap-1 rounded-lg border border-neutral-700/60 bg-neutral-800 p-1 ${className}`}
    >
      {children}
    </div>
  );
}

export function TabsTrigger({ value, children }) {
  const { current, set } = useContext(TabsCtx);
  const active = current === value;
  return (
    <button
      onClick={() => set(value)}
      className={`rounded px-3 py-1 text-sm transition
        ${active ? "bg-red-600 text-white" : "text-neutral-400 hover:text-red-400"}`}
    >
      {children}
    </button>
  );
}

export function TabsContent({ value, children, className = "" }) {
  const { current } = useContext(TabsCtx);
  return current === value ? <div className={className}>{children}</div> : null;
}
