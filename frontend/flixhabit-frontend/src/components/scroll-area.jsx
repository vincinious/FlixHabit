import React from "react";

export function ScrollArea({ children, className = "" }) {
  return (
    <div
      className={`overflow-y-auto scrollbar-thin scrollbar-track-neutral-900
                  scrollbar-thumb-neutral-700/60 ${className}`}
    >
      {children}
    </div>
  );
}
